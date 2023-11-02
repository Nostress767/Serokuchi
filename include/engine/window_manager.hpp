#pragma once

#include "types.hpp"
#include "window.hpp"

class WindowManager{
private:
  HMODULE user32Library = NULL;
  HMODULE xInputLibrary = NULL;

  BOOL (WINAPI *setProcessDPIAware)() = NULL;
  DWORD (WINAPI *xInputGetState)(DWORD, XINPUT_STATE*) = NULL;
  DWORD (WINAPI *xInputSetState)(DWORD, XINPUT_VIBRATION*) = NULL;

  WNDCLASS windowClass = {};

  std::unique_ptr<Window> root = nullptr;
  std::map<std::wstring, std::shared_ptr<Window>> windows;
  std::map<i32, std::shared_ptr<Window>> windowsDrawingOrder;

  bool isFullscreen = false, isRunning = false;

  Clock clk = {};
  std::array<Key, 256> K = {};
  std::array<Joystick, 4> J = {};

public:
  WindowManager(i32 sizeX, i32 sizeY, std::wstring name);

  void resetClock();

  template<typename Time> double getTotalElapsed();
  template<typename Time> double getLastFrameElapsed();
  template<typename Time> double getLastSecondElapsed();

  const Key& operator()(SpecialKey spKey) const;
  const Key& operator()(char key) const;
  const std::shared_ptr<Window> operator[](std::wstring windowKey);

  void updateJoystickState();
  void sendRumble(i32 controller, u16 left, u16 right);
  void removeWindow(std::wstring windowKey);
  void createWindow(i32 windowZOrder, i32 windowSizeX, i32 windowSizeY, std::wstring windowTitle);
  void draw();
  void drawWindows();
  void drawAll();
  bool shouldRun();
  void endFrame();
  void updateKeyState(u32 key, u32 bitfield);
  void beginFrame();
  u64 getFrame();
  u64 getTotalFrames();
  u64 getFPS();
  double getElapsedTime();
  Clock& tickClock();
};


