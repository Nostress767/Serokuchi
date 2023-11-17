#pragma once

#include "assets/png_media.hpp"
#include "engine/clock.hpp"

#include <map>

// PNG-type sprite using stb_image's API
struct Sprite {
  const PngMedia* spriteData;
/// NOTE: from notes
  i32 x, y;
  i32 originalSizeX, originalSizeY;
  i32 sizeX, sizeY;

  bool changed = true;

  i32 alpha = 255;
  bool visible = true;

  // TODO
  bool animated = false;
  i32 animationCurrentStep = 0;
  std::pair<i32, i32> animationCycle = {0, 0};
  // NOTE: is this needed?
  //Clock animationTimer;
///

  Sprite(PngMedia& im, i32 width, i32 height, i32 x, i32 y);
  virtual void draw(HDC windowBaseBitmapDC, HDC spriteBitmapDC);
  
  virtual void moveTo(i32 x, i32 y);
  virtual void moveToRelative(i32 x, i32 y);
  void scale(i32 scaleFactor);
  void scale(i32 scaleFactorX, i32 scaleFactorY);
  void scale(f32 scaleFactor);
  void scale(f32 scaleFactorX, f32 scaleFactorY);
  void setAlpha(i32 alpha);
  void setVisibility(bool value);
  void toggleVisibility();
  void stepAnimationForwards();
  void stepAnimationBackwards();
  void setAnimationCycle(i32 start, i32 end);
  //void tickAnimation();
  void setAnimationStep(i32 step);
};
