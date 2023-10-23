#pragma once

#include <string>

struct image
  {
  int w = 0;
  int h = 0;
  int nr_of_channels = 0;
  unsigned char* im = nullptr;
  };

image read_image(const std::string& filename);
void destroy_image(image& im);
bool is_valid(const image& im);