#include <iostream>
#include <string>
#include <sstream>

#define JTK_FILE_UTILS_IMPLEMENTATION
#include "jtk/file_utils.h"

#include "image.h"
#include "video_writer.h"


void sort_all_files_based_on_numbers_in_their_filenames(std::vector<std::string>& files)
  {
  std::vector<double> numbers;
  numbers.reserve(files.size());
  std::vector<uint64_t> indices;
  indices.reserve(files.size());
  for (uint64_t i = 0; i < files.size(); ++i)
    indices.push_back(i);
  for (const auto& f : files)
    {
    std::string fn = jtk::remove_extension(jtk::get_filename(f));
    std::string nr;
    bool dot = false;
    auto rit = fn.rbegin();
    auto rend = fn.rend();
    bool digit_found = false;
    while (rit != rend)
      {
      if (std::isdigit(*rit))
        {
        digit_found = true;
        nr.push_back(*rit);
        }
      else
        {
        if (*rit == '.' && digit_found && !dot)
          {
          dot = true;
          nr.push_back(*rit);
          }
        else
          {
          if (digit_found)
            break;
          }
        }
      ++rit;
      }
    if (!digit_found)
      numbers.push_back(std::numeric_limits<double>::max());
    else
      {
      if (dot && nr.back() == '.')
        nr.push_back('0');
      std::reverse(nr.begin(), nr.end());
      std::stringstream str;
      str << nr;
      double d;
      str >> d;
      numbers.push_back(d);
      }
    }
  std::sort(indices.begin(), indices.end(), [&](int i, int j)
    {
    return numbers[i] < numbers[j];
    });
  std::vector<std::string> sorted_files;
  sorted_files.reserve(files.size());
  for (auto i : indices)
    {
    sorted_files.push_back(files[i]);
    }
  files.swap(sorted_files);
  }

struct app_options
  {
  std::string image_extension = std::string("png");
  };

app_options process_options(const std::vector<std::string>& ops)
  {
  app_options options;
  for (const auto& s : ops)
    {
    if (s == std::string("-png"))
      options.image_extension = std::string("png");
    if (s == std::string("-bmp"))
      options.image_extension = std::string("bmp");
    if (s == std::string("-jpg"))
      options.image_extension = std::string("jpg");
    if (s == std::string("-jpeg"))
      options.image_extension = std::string("jpeg");
    }
  return options;
  }

void process(const std::string& images_folder, const std::string& video_file, const std::vector<std::string>& ops)
  {
  auto options = process_options(ops);
  auto files = jtk::get_files_from_directory(images_folder, false);
  sort_all_files_based_on_numbers_in_their_filenames(files);
  int index = 0;
  VideoWriterState state;
  for (auto& f : files)
    {
    std::string ext = jtk::get_extension(f);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](char ch){ return ::tolower(ch);});
    if (ext == options.image_extension)
      {
      std::cout << f << std::endl;
      image im = read_image(f);
      if (is_valid(im))
        {
        if (index == 0)
          {
          if (!initialize(state, video_file, im.w, im.h))
            return;
          }
        push_frame(state, im);
        ++index;
        destroy_image(im);
        }
      }
    }
  finish(state);
  free(state);
  }

int main(int argc, char** argv)
  {
  if (argc < 3)
    {
    std::cout << "Usage: Images2Video <images folder> <video file> [options]" << std::endl;
    std::cout << "  [options] = -png | -bmp | -jpg | -jpeg";
    }
  else
    {
    std::string images_folder(argv[1]);
    std::string video_file(argv[2]);
    std::cout << "Reading images from folder " << images_folder << std::endl;
    std::cout << "Writing to video file " << video_file << std::endl;
    std::vector<std::string> options;
    for (int i = 3; i < argc; ++i)
      options.emplace_back(argv[i]);
    process(images_folder, video_file, options);
    }
  return 0;
  }