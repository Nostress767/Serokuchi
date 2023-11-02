#pragma once

#define UNICODE
#include <xinput.h>

#include <array>

#include "types.hpp"

enum class SpecialKey : u32 {
  escape = 128, enter, tab, backspace, insert, deleto,
  right, left, down, up,
  pageUp, pageDown, home, end,
  capslock, scrollLock, numLock, printScreen, pauseBreak,
  f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
  numpad0, numpad1, numpad2, numpad3, numpad4, numpad5, numpad6, numpad7, numpad8, numpad9,
  decimal, divide, multiply, subtract, add,
  leftShift, leftCtrl, leftAlt, leftSuper,
  rightShift, rightCtrl, rightAlt, rightSuper
};

// NOTE: the choice for namespace enum instead of enum class is because I use the enum value (and don't want to cast every time)
namespace JoystickButton {
enum {
  dpadUp, dpadDown, dpadLeft, dpadRight,
  start, back,
  leftThumb, rightThumb,
  leftShoulder, rightShoulder,
  a, b, x, y,
  dualShockLeftTrigger, dualShockRightTrigger,
  dualShockHome
};
};

namespace JoystickAxis {
enum {
  leftThumbX, leftThumbY, leftTrigger,
  rightThumbX, rightThumbY, rightTrigger
};
};

struct Mouse {
  i32 x = 0, y = 0;
  i32 capturedX = 0, capturedY = 0;
  bool leftPressed = false, middlePressed = false, rightPressed = false;
  bool leftHeld = false, middleHeld = false, rightHeld = false;
  bool leftReleased = false, middleReleased = false, rightReleased = false;
  bool hasFocus = false;
};

struct Key {
  bool isPressed = false, isHeld = false, isReleased = false;
};

// TODO: maybe joysticks should also have repeating (like keys)?
struct Joystick {
  XINPUT_STATE jsState = {};
  // f64
  // u8 timer;
  //char name[64];
  bool isOn = false;
  i32 axesN = 6;
  i32 buttonsN = 14;
  std::array<Key, 32> button = {};
  std::array<i32, 16> axis = {};
};

