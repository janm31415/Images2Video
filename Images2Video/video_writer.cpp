#include "video_writer.h"
#include <iostream>
#include <cassert>

bool initialize(VideoWriterState& state, const std::string& filename, int w, int h)
  {
  state.width = w;
  state.height = h;
  state.oformat = av_guess_format(nullptr, filename.c_str(), nullptr);
  if (!state.oformat)
    {
    std::cout << "can't create output format" << std::endl;
    return false;
    }

  int err = avformat_alloc_output_context2(&state.ofctx, state.oformat, nullptr, filename.c_str());
  if (err)
    {
    std::cout << "can't create output context" << std::endl;
    return false;
    }

  state.codec = avcodec_find_encoder(state.oformat->video_codec);
  if (!state.codec)
    {
    std::cout << "can't create codec" << std::endl;
    return false;
    }

  state.stream = avformat_new_stream(state.ofctx, state.codec);
  if (!state.stream)
    {
    std::cout << "can't find format" << std::endl;
    return false;
    }

  state.cctx = avcodec_alloc_context3(state.codec);
  if (!state.cctx)
    {
    std::cout << "can't create codec context" << std::endl;
    return false;
    }

  state.stream->codecpar->codec_id = state.oformat->video_codec;
  state.stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  state.stream->codecpar->width = w;
  state.stream->codecpar->height = h;
  state.stream->codecpar->format = AV_PIX_FMT_YUV420P;
  state.stream->codecpar->bit_rate = state.bitrate * 1000;
  avcodec_parameters_to_context(state.cctx, state.stream->codecpar);
  state.cctx->time_base.num = 1;
  state.cctx->time_base.den = 1;
  state.cctx->max_b_frames = 2;
  state.cctx->gop_size = 12;
  state.cctx->framerate.num = state.fps;
  state.cctx->framerate.den = 1;
  //must remove the following
  //cctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  if (state.stream->codecpar->codec_id == AV_CODEC_ID_H264) {
    av_opt_set(state.cctx, "preset", "ultrafast", 0);
    }
  else if (state.stream->codecpar->codec_id == AV_CODEC_ID_H265)
    {
    av_opt_set(state.cctx, "preset", "ultrafast", 0);
    }

  avcodec_parameters_from_context(state.stream->codecpar, state.cctx);

  if ((err = avcodec_open2(state.cctx, state.codec, NULL)) < 0) {
    std::cout << "Failed to open codec" << err << std::endl;
    return false;
    }

  if (!(state.oformat->flags & AVFMT_NOFILE)) {
    if ((err = avio_open(&state.ofctx->pb, filename.c_str(), AVIO_FLAG_WRITE)) < 0) {
      std::cout << "Failed to open file" << err << std::endl;
      return false;
      }
    }

  if ((err = avformat_write_header(state.ofctx, NULL)) < 0) {
    std::cout << "Failed to write header" << err << std::endl;
    return false;
    }

  av_dump_format(state.ofctx, 0, filename.c_str(), 1);

  return true;
  }

void push_frame(VideoWriterState& state, const image& im)
  {
  assert(is_valid(im));
  int err;
  if (!state.videoFrame) {
    state.videoFrame = av_frame_alloc();
    state.videoFrame->format = AV_PIX_FMT_YUV420P;
    state.videoFrame->width = state.cctx->width;
    state.videoFrame->height = state.cctx->height;
    if ((err = av_frame_get_buffer(state.videoFrame, 32)) < 0) {
      std::cout << "Failed to allocate picture" << err << std::endl;
      return;
      }
    }
  if (!state.swsCtx) {
    state.swsCtx = sws_getContext(state.cctx->width, state.cctx->height, AV_PIX_FMT_RGB24, state.cctx->width,
      state.cctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
    }
  int inLinesize[1] = { 3 * state.cctx->width };
  // From RGB to YUV
  sws_scale(state.swsCtx, (const uint8_t* const*)&im.im, inLinesize, 0, state.cctx->height,
    state.videoFrame->data, state.videoFrame->linesize);
  state.videoFrame->pts = (1.0 / 30.0) * 90000 * (state.frameCounter++);
  std::cout << state.videoFrame->pts << " " << state.cctx->time_base.num << " " <<
    state.cctx->time_base.den << " " << state.frameCounter << std::endl;
  if ((err = avcodec_send_frame(state.cctx, state.videoFrame)) < 0) {
    std::cout << "Failed to send frame" << err << std::endl;
    return;
    }
  AV_TIME_BASE;
  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;
  pkt.flags |= AV_PKT_FLAG_KEY;
  if (avcodec_receive_packet(state.cctx, &pkt) == 0) {
    static int counter = 0;
    //if (counter == 0) {
    //  FILE* fp = fopen("dump_first_frame1.dat", "wb");
    //  fwrite(pkt.data, pkt.size, 1, fp);
    //  fclose(fp);
    //  }
    std::cout << "pkt key: " << (pkt.flags & AV_PKT_FLAG_KEY) << " " <<
      pkt.size << " " << (counter++) << std::endl;
    uint8_t* size = ((uint8_t*)pkt.data);
    std::cout << "first: " << (int)size[0] << " " << (int)size[1] <<
      " " << (int)size[2] << " " << (int)size[3] << " " << (int)size[4] <<
      " " << (int)size[5] << " " << (int)size[6] << " " << (int)size[7] <<
      std::endl;
    av_interleaved_write_frame(state.ofctx, &pkt);
    av_packet_unref(&pkt);
    }
  }

void finish(VideoWriterState& state)
  {
  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;

  for (;;) {
    avcodec_send_frame(state.cctx, NULL);
    if (avcodec_receive_packet(state.cctx, &pkt) == 0) {
      av_interleaved_write_frame(state.ofctx, &pkt);
      av_packet_unref(&pkt);
      }
    else {
      break;
      }
    }

  av_write_trailer(state.ofctx);
  if (!(state.oformat->flags & AVFMT_NOFILE)) {
    int err = avio_close(state.ofctx->pb);
    if (err < 0) {
      std::cout << "Failed to close file" << err << std::endl;
      }
    }
  }

void free(VideoWriterState& state)
  {
  if (state.videoFrame) {
    av_frame_free(&state.videoFrame);
    }
  if (state.cctx) {
    avcodec_free_context(&state.cctx);
    }
  if (state.ofctx) {
    avformat_free_context(state.ofctx);
    }
  if (state.swsCtx) {
    sws_freeContext(state.swsCtx);
    }
  }