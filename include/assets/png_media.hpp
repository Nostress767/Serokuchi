#pragma once

#include <vector>

#include "media.hpp"

class PngMedia : private Media{
private:
  std::vector<std::vector<u8>> data;
  i32 width, height, channels;

public:
  PngMedia(u8* bin, size_t sz);
  ~PngMedia();

  std::vector<std::vector<u8>>& getData();
  i32 getWidth();
  i32 getHeight();
  i32 getChannels();
  void load();
  void decode();
  void unload();
};

#define ___include_png(filename, extension) \
  media_binary(filename, extension) \
  extern PngMedia filename

#define ___define_png(filename, extension) \
  PngMedia filename (media_start(filename, extension), media_size(filename, extension))

