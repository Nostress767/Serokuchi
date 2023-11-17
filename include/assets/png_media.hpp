#pragma once

#include <memory>
#include <vector>

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "engine/clock.hpp"
#include "media.hpp"

// NOTE: should only be used with unique_ptr (because of destructor)
struct BitmapData{
  const i32 bitmapWidth;
  const i32 bitmapHeight;
  const HDC bitmapDC;
  const HBITMAP bitmap;
  const HBITMAP defaultBitmap;

  BitmapData(i32 bitmapWidth, i32 bitmapHeight, HDC bitmapDC, HBITMAP bitmap, HBITMAP defaultBitmap);
  ~BitmapData();
};

class PngMedia : private Media{
public:
  PngMedia(u8* bin, size_t sz);

  std::unique_ptr<BitmapData> getBitmapData(HWND hWnd);
};

#define ___include_png(filename, extension) \
  media_binary(filename, extension) \
  extern PngMedia filename

#define ___define_png(filename, extension) \
  PngMedia filename (media_start(filename, extension), media_size(filename, extension))

