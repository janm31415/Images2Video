#pragma once

#include "image.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
  }

#include <string>

/*
* source: https://stackoverflow.com/questions/46444474/c-ffmpeg-create-mp4-file
*/

struct VideoWriterState {
  int width = 0;
  int height = 0;
  AVFrame* videoFrame = nullptr;
  AVCodecContext* cctx = nullptr;
  SwsContext* swsCtx = nullptr;
  int frameCounter = 0;
  AVFormatContext* ofctx = nullptr;
  const AVOutputFormat* oformat = nullptr;
  int fps = 30;
  int bitrate = 2000;
  const AVCodec* codec = nullptr;
  AVStream* stream = nullptr;
  };

bool initialize(VideoWriterState& state, const std::string& filename, int w, int h);

void push_frame(VideoWriterState& state, const image& im);

void finish(VideoWriterState& state);

void free(VideoWriterState& state);