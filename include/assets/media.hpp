#pragma once

#include "types.hpp"

class Media{
private: 
  const u8 *binStart;
  const size_t len;
public:
  Media(u8 *binStart, size_t len);

  const u8* getRawData();
  const size_t size();
};

#define media_start(filename, extension) binary_##filename##_##extension##_start
#define media_end(filename, extension) binary_##filename##_##extension##_end
#define media_binary(filename, extension) \
  extern u8 media_start(filename, extension)[]; \
  extern u8 media_end(filename, extension)[];
#define media_size(filename, extension) media_end(filename, extension) - media_start(filename, extension)

