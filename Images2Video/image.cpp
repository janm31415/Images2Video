#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

image read_image(const std::string& filename)
  {
  image im;
  im.im = stbi_load(filename.c_str(), &im.w, &im.h, &im.nr_of_channels, 3);
  return im;
  }

void destroy_image(image& im)
  {
  if (im.im)
    {
    stbi_image_free(im.im);
    im.im = nullptr;
    im.w = 0;
    im.h = 0;
    im.nr_of_channels = 0;
    }
  }

bool is_valid(const image& im)
  {
  return im.im != nullptr;
  }