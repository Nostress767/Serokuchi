#include "engine/window.hpp"
#include <algorithm>

Window::Window(WNDCLASS &wndClass, i32 sizeX, i32 sizeY, std::wstring name, i32 zOrder, WindowType wndType){
  title = name;
  currentPosX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (sizeX / 2);
  currentPosY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (sizeY / 2);
  width = previousWidth = drawingWidth = sizeX;
  height = previousHeight = drawingHeight = sizeY;
  offsetX = 0; offsetY = 0;
  this->zOrder = zOrder;

  DWORD style = WS_POPUP | WS_VISIBLE;
  RECT rect = {0, 0, sizeX, sizeY};
  AdjustWindowRectEx(&rect, style, false, 0);

  DWORD exStyle = WS_EX_LAYERED;
  if(wndType != WindowType::root)
    exStyle |= WS_EX_TOOLWINDOW;

  hWnd = CreateWindowEx(exStyle, wndClass.lpszClassName, title.c_str(), style,
    // WS_CAPTION | WS_VISIBLE | WS_SYSMENU <- creates a window with only menu and X buttons
    // WS_CAPTION | WS_VISIBLE | WS_POPUP <- window has a top frame but no buttons at all
    // | WS_DISABLED, //WS_CLIPCHILDREN | WS_CLIPSIBLINGS
    currentPosX, currentPosY,
    rect.right - rect.left, rect.bottom - rect.top,
    0, 0, wndClass.hInstance, &this->zOrder);

  if(hWnd){
    log_debug("Created window \"" << title << "\" (" << hWnd << ")");
  }

  bitmapMemory = std::make_unique<u32[]>(drawingWidth * drawingHeight);
  isRunning = true;
  SetWindowLongPtr(hWnd, 0, (LONG)(this));
}

bool Window::isFocused(){ return hasFocus; }

void Window::draw(){
  if(nothingChanged)
    return;

  // NOTE: could be optimized (reduce the number of calls required/avoid unnecessary calls)
  HDC hDc = GetDC(hWnd);

  HDC baseBitmapDC = CreateCompatibleDC(hDc);
  HBITMAP baseBitmap = CreateCompatibleBitmap(hDc, drawingWidth, drawingHeight);
  HBITMAP defaultBitmap1 = (HBITMAP)SelectObject(baseBitmapDC, baseBitmap);

  HDC stretchedBitmapDC = CreateCompatibleDC(NULL);
  HBITMAP stretchedBitmap = CreateCompatibleBitmap(hDc, width, height);
  HBITMAP defaultBitmap2 = (HBITMAP)SelectObject(stretchedBitmapDC, stretchedBitmap);
  
  BITMAPINFO bitmapInfo = {};
  bitmapInfo =
    (BITMAPINFO){.bmiHeader = {.biSize = sizeof(BITMAPINFOHEADER),
                               .biWidth = drawingWidth,
                               .biHeight = drawingHeight,
                               .biPlanes = 1,
                               .biBitCount = 32,
                               .biCompression = BI_RGB}};

  SetDIBits(hDc, baseBitmap, 0, drawingHeight, bitmapMemory.get(), &bitmapInfo, DIB_RGB_COLORS);
  StretchBlt(stretchedBitmapDC, 0, 0, width, height, baseBitmapDC, 0, 0, drawingWidth, drawingHeight, SRCCOPY);

  BLENDFUNCTION blendFunc = {};
  blendFunc.BlendOp = AC_SRC_OVER;
  blendFunc.SourceConstantAlpha = 255;
  blendFunc.AlphaFormat = AC_SRC_ALPHA;

  POINT	pSrc = { 0, 0 };
  SIZE	sizeWnd = { width, height };

  UpdateLayeredWindow(hWnd, hDc, NULL, &sizeWnd, stretchedBitmapDC, &pSrc, 0, &blendFunc, ULW_ALPHA);

  SelectObject(baseBitmapDC, defaultBitmap1);
  DeleteObject(baseBitmap);
  DeleteDC(baseBitmapDC);
  SelectObject(stretchedBitmapDC, defaultBitmap2);
  DeleteObject(stretchedBitmap);
  DeleteDC(stretchedBitmapDC);

  ReleaseDC(hWnd, hDc);
  nothingChanged = true;
}

void Window::endFrame() {
  if(mouse.leftPressed)
    mouse.leftHeld = true;
  if(mouse.middlePressed)
    mouse.middleHeld = true;
  if(mouse.rightPressed)
    mouse.rightHeld = true;

  mouse.leftPressed = mouse.rightPressed = false;
  mouse.leftReleased = mouse.rightReleased = false;
  mouse.middlePressed = mouse.middleReleased = false;
}

void Window::c(u32 color) {
  std::fill_n(bitmapMemory.get(), drawingWidth * drawingHeight, color);
  nothingChanged = false;
}

void Window::d(i32 x, i32 y, u32 color) {
  // TODO: investigate transparency bug
  if(x < 0 || y < 0 || x >= drawingWidth || y >= drawingHeight)// || (color & 0xFF000000) == 0)
    return;
  bitmapMemory[y * drawingWidth + x] = color;
  nothingChanged = false;
}

void Window::setWindowSize(i32 sizeX, i32 sizeY) {
  width = sizeX;
  height = sizeY;
  nothingChanged = false;
  draw();
}

// TODO: reinvestigate these two functions
// NOTE: fonts are assumed to be:
// 16 characters across by 6 standing
// hence the division by 16 and 6 on width and height
// (since we load the entire image and only draw parts of it)
void Window::drawText(PngMedia& png, i32 x, i32 y, std::string text, u32 color, i32 scale, i32 space_x, i32 space_y){
  if(png.getData().empty())
    png.load();

  i32 text_x = 0, text_y = 0;
  size_t str_len = text.length();

  for(size_t i = 0; i < str_len; i++){
    if(text[i] == '\n'){
      text_x = 0; text_y -= png.getHeight() / 6 * scale + space_y;}
    else{
      u32 char_index_x = (text[i] - 32) % 16, char_index_y = (text[i] - 32) / 16;
      if(scale > 1){
        for(i32 j = 0; j < png.getWidth() / 16; j++)
          for(i32 k = 0; k < png.getHeight() / 6; k++)
            if(png.getData()[k + char_index_y * png.getHeight() / 6][(j + char_index_x * png.getWidth() / 16) * 4])
              for (i32 scaled_i = 0; scaled_i < scale; scaled_i++)
                for (i32 scaled_j = 0; scaled_j < scale; scaled_j++)
                  d(x + text_x + (j * scale) + scaled_i, y + text_y + (k * scale) + scaled_j, color);
      }
      else{
        for(i32 j = 0; j < png.getWidth() / 16; j++)
          for(i32 k = 0; k < png.getHeight() / 6; k++)
            if(png.getData()[k + char_index_y * png.getHeight() / 6][(j + char_index_x * png.getWidth() / 16) * 4])
              d(x + text_x + j, y + text_y + k, color);}
      text_x += (png.getWidth() / 16 + space_x) * scale ;}}
}

void Window::drawSprite(Sprite *spr) {
  for (i32 i = 0; i < spr->height; i++)
    for (i32 j = 0; j < spr->width; j++)
      d(spr->x + j, spr->y + i, spr->data[i][j * 4 + 3] << 24 | spr->data[i][j * 4] << 16 | spr->data[i][j * 4 + 1] << 8 | spr->data[i][j * 4 + 2] << 0);
}
