#pragma once

#include "assets/png_media.hpp"

// PNG-type sprite using stb_image's API
struct Sprite {
  i32 x, y;
  i32 width, height, channels;

  std::vector<std::vector<u8>> data;

  Sprite(PngMedia *im);
};
