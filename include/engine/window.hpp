#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <iostream>

#include "assets/ogg_media.hpp"
#include "assets/png_media.hpp"
#include "clock.hpp"
#include "input.hpp"
#include "sprite.hpp"

#define ARGB(a, r, g, b) ((a << 24) | (r << 16) | (g << 8) | b)

enum class WindowType { root, other };
enum PixelColorChannel { alphaChannel = 0xFF000000, redChannel = 0xFF0000, greenChannel = 0xFF00, blueChannel = 0xFF };

struct Window {
  inline static std::map<i32, HWND> windowOrder;
  
  HWND hWnd = 0;

  std::wstring title;

  i32 width = -1, drawingWidth = -1, previousWidth = -1;
  i32 height = -1, drawingHeight = -1, previousHeight = -1;
  i32 currentPosX = -1, currentPosY = -1;
  i32 offsetX = -1, offsetY = -1;
  i32 zOrder = -1;
  // TODO: implement lockWindowMovement using isMovable
  bool isRunning = false, hasFocus = false, isFullscreen = false, isMovable = false, isMoving = false, newDrawing = true;

  Mouse mouse = {};

  std::unique_ptr<u32[]> bitmapMemory = nullptr;

  // NOTE: could be used if more performance is required
  HDC baseBitmapDC = NULL;
  HBITMAP baseBitmap = {};
  HBITMAP defaultBitmap1 = {};

  HDC stretchedBitmapDC = NULL;
  HBITMAP stretchedBitmap = {};
  HBITMAP defaultBitmap2 = {};

  Window(WNDCLASS &wndClass, i32 sizeX, i32 sizeY, i32 scale, std::wstring name, i32 zOrder, WindowType wndType = WindowType::other);

  bool isFocused();
  //void beginFrame();
  void draw();
  void endFrame();
  void c(u32 color);
  void d(i32 x, i32 y, u32 color, bool zeroAlpha = false);
  void dSquare(i32 size, i32 x, i32 y, u32 color, bool zeroAlpha = false);
  void setWindowSize(i32 sizeX, i32 sizeY);

  // TODO: reinvestigate these two functions
  // NOTE: fonts are assumed to be:
  // 16 characters across by 6 standing
  // hence the division by 16 and 6 on width and height
  // (since we load the entire image and only draw parts of it)
  void drawText(PngMedia& spr, i32 x, i32 y, std::string text, u32 color, i32 scale, i32 spaceX, i32 spaceY);
  void dSprite(Sprite *spr);
};

