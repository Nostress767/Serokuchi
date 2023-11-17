#include "engine/sprite.hpp"

Sprite::Sprite(PngMedia& im, i32 width, i32 height, i32 x, i32 y) : spriteData(&im){
  sizeX = originalSizeX = width;
  sizeY = originalSizeY = height;
  this->x = x;
  this->y = y;
}

void Sprite::draw(HDC windowBaseBitmapDC, HDC spriteBitmapDC){
  //log_info("Drawing sprite: " << x << " - " << y << " " << sizeX << " - " << sizeY << " " << originalSizeX << " - " << originalSizeY);
  StretchBlt(windowBaseBitmapDC, x, y, sizeX, sizeY, spriteBitmapDC, 0, 0, originalSizeX, originalSizeY, SRCCOPY);
  changed = false;
}

void Sprite::moveTo(i32 x, i32 y){
  this->x = x;
  this->y = y;
}

void Sprite::moveToRelative(i32 x, i32 y){
  this->x += x;
  this->y += y;
}

void Sprite::scale(i32 scaleFactor){
  scale(scaleFactor, scaleFactor);
}

void Sprite::scale(i32 scaleFactorX, i32 scaleFactorY){
  if(scaleFactorX < 1 || scaleFactorY < 1){
    log_error("Scale factors cannot be smaller than 1: " << scaleFactorX << " - " << scaleFactorY);
  }
  else{
    sizeX = originalSizeX * scaleFactorX;
    sizeY = originalSizeY * scaleFactorY;
  }
}

void Sprite::scale(f32 scaleFactor){
  scale(scaleFactor, scaleFactor);
}

void Sprite::scale(f32 scaleFactorX, f32 scaleFactorY){
  if(scaleFactorX < 1 || scaleFactorY < 1){
    log_error("Scale factors cannot be smaller than 1: " << scaleFactorX << " - " << scaleFactorY);
  }
  else{
    sizeX = static_cast<i32>(originalSizeX * scaleFactorX);
    sizeY = static_cast<i32>(originalSizeY * scaleFactorY);
  }
}

void Sprite::setAlpha(i32 alpha){
  if(alpha == 0)
    setVisibility(false);
  else if(alpha > 0 && alpha < 256)
    this->alpha = alpha;
  else{
    log_error("Invalid alpha value: " << alpha);
  }
}

void Sprite::setVisibility(bool value){
  visible = value;
}

void Sprite::toggleVisibility(){
  visible = !visible;
}

void Sprite::stepAnimationForwards(){
  if(animationCurrentStep == animationCycle.second){
    animationCurrentStep = animationCycle.first;
  }
  else{
    animationCurrentStep++;
  }
}

void Sprite::stepAnimationBackwards(){
  if(animationCurrentStep == animationCycle.first){
    animationCurrentStep = animationCycle.second;
  }
  else{
    animationCurrentStep--;
  }
}

void Sprite::setAnimationCycle(i32 start, i32 end){
  if(start > end || start < 0 || end < 0){
    log_error("Invalid animation start/end value: " << start << " - " << end);
    return;
  }
  animationCycle.first = start;
  animationCycle.second = end;
}

//void Sprite::tickAnimation(){}

void Sprite::setAnimationStep(i32 step){
  if(animationCycle.first <= step && step <= animationCycle.second){
    animationCurrentStep = step;
  }
  else{
    log_error("Invalid animation step: " << step << "(start=" << animationCycle.first << ", end=" << animationCycle.second << ")");
  }
}

