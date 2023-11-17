#include "assets/png_media.hpp"

#include "external/stb_image.h"

#include <numeric>

BitmapData::BitmapData(i32 bitmapWidth, i32 bitmapHeight, HDC bitmapDC, HBITMAP bitmap, HBITMAP defaultBitmap) : bitmapWidth(bitmapWidth), bitmapHeight(bitmapHeight), bitmapDC(bitmapDC), bitmap(bitmap), defaultBitmap(defaultBitmap) {}

BitmapData::~BitmapData(){
  SelectObject(bitmapDC, defaultBitmap);
  DeleteObject(bitmap);
  DeleteDC(bitmapDC);
}

PngMedia::PngMedia(u8* bin, size_t sz) : Media(bin, sz) {}

// TODO: replace this with std::shared_ptr
std::unique_ptr<BitmapData> PngMedia::getBitmapData(HWND hWnd){

  i32 width, height, channels;
  u8 *reverseRows = stbi_load_from_memory(getRawData(), size(), &width, &height, &channels, 4);

  //log_debug("Loaded PNG image has: width=" << width << ", height=" << height << ", channels=" << channels);
  std::unique_ptr<i32[]> data = std::make_unique<i32[]>(width * height);

  for(i32 i = 0; i < height; i++){
    for(i32 j = 0; j < width; j++){
      /// NOTE: this mirrors the image vertically
      i32 input_index = ((height - 1 - i) * width + j) * 4;
      ///
      // NOTE: this engine uses ARGB
      // PNG raw data is ABGR
      // byteswap would make it RGBA
      // rotr (of one byte) would make it ARGB
      data[i * width + j] = std::rotr(std::byteswap(*reinterpret_cast<u32*>(reverseRows + input_index)), 8);
    }
  }

  stbi_image_free(reverseRows);

  HDC hDc = GetDC(hWnd);

  HDC bitmapDC = CreateCompatibleDC(hDc);
  HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
  HBITMAP defaultBitmap = (HBITMAP)SelectObject(bitmapDC, bitmap);

  BITMAPINFO bitmapInfo = {};
  bitmapInfo =
    (BITMAPINFO){.bmiHeader = {.biSize = sizeof(BITMAPINFOHEADER),
                               .biWidth = width,
                               // NOTE: negative here means mirroring vertically
                               .biHeight = height,
                               .biPlanes = 1,
                               .biBitCount = 32,
                               .biCompression = BI_RGB}};

  SetDIBits(hDc, bitmap, 0, height, data.get(), &bitmapInfo, DIB_RGB_COLORS);

  ReleaseDC(hWnd, hDc);

  return std::make_unique<BitmapData>(width, height, bitmapDC, bitmap, defaultBitmap);
}

