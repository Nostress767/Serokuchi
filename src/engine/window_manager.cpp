//#define show_message_loop
#include "engine/window_manager.hpp"

WindowManager::WindowManager(std::wstring name, i32 sizeX, i32 sizeY){
  K = std::make_unique<Key[]>(256);
  J = std::make_unique<Joystick[]>(4);
  log_debug("Args: (sizeX : " << sizeX << ", sizeY : " << sizeY << ", name : " << name << ")");
  isRunning = true;

  user32Library = LoadLibraryA("user32.dll");
  if(user32Library){
    log_debug("Loaded library user32.dll");
    setProcessDPIAware = (BOOL (WINAPI *)())GetProcAddress(user32Library, "SetProcessDPIAware");
  }
  else{
    log_error("Couldn't load library user32.dll");
  }

  if(setProcessDPIAware){
    log_debug("Loaded function SetProcessDPIAware");
    setProcessDPIAware();
  }
  else{
    log_error("Couldn't load function SetProcessDPIAware");
  }

  xInputLibrary = LoadLibraryA("xinput1_3.dll");
  if(xInputLibrary){
    log_debug("Loaded library xinput1_3.dll");

    xInputGetState = (DWORD (WINAPI *)(DWORD, XINPUT_STATE*))GetProcAddress(xInputLibrary, "XInputGetState");
    if(xInputGetState){
      log_debug("Loaded function XInputGetState");
    }
    else{
      log_error("Couldn't load function XInputGetState");
    }

    xInputSetState = (DWORD (WINAPI *)(DWORD, XINPUT_VIBRATION*))GetProcAddress(xInputLibrary, "XInputSetState");
    if(xInputSetState){
      log_debug("Loaded function XInputSetState");
    }
    else{
      log_error("Couldn't load function XInputSetState");
    }
  }

  windowClass.lpfnWndProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT CALLBACK {
    log_message("lpfnWndProc args", "(hWnd : " << hWnd << ", message : " << message << ", wParam : " << wParam << ", lParam : " << lParam << ")");
    Window* window = (Window*)(GetWindowLongPtr(hWnd, 0));

    switch (message) {
      case WM_NCCREATE: {
        CREATESTRUCT *cs = (CREATESTRUCT *)(lParam);
        Window::windowOrder[*(i32*)(cs->lpCreateParams)] = hWnd;
      } break;
      // XXX
      //case WM_SETCURSOR: {
      //  // Removes loading cursor
      //  SetCursor(LoadCursor(NULL, IDC_ARROW));
      //  // Returning 0 stops beeping (clicking inactive window)
      //} return 0;
      case WM_ACTIVATE: {
        if(window){
          if(LOWORD(wParam) == WA_CLICKACTIVE){
            for(auto &[_, wHandle] : Window::windowOrder)
              SetWindowPos(wHandle, HWND_TOP, -1, -1, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
          }
          else if(LOWORD(wParam) == WA_ACTIVE){
            // NOTE: root has zOrder 0
            // NOTE: is it bad to comment this out?
            //if(Window::windowOrder.contains(0) && Window::windowOrder[0] == hWnd)
              for(auto &[_, wHandle] : Window::windowOrder)
                SetWindowPos(wHandle, HWND_TOP, -1, -1, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
          }
        }
      } break;
      case WM_SIZE: {
        if(window){
          window->width = GET_X_LPARAM(lParam);
          window->height = GET_Y_LPARAM(lParam);
          log_message("WM_SIZE", "Window \"" << window->title << "\" (" << hWnd << ") has changed size to " << window->width << "x" << window->height);
        }
      } break;
      case WM_MOUSEMOVE: {
        if(window){
          window->mouse.hasFocus = true;
          window->mouse.x = GET_X_LPARAM(lParam);
          window->mouse.y = GET_Y_LPARAM(lParam);
          log_message("WM_MOUSEMOVE", "Window \"" << window->title << "\" (" << hWnd << ") mouse has changed position to (" << window->mouse.x << ", " << window->mouse.y << ")");

          if (!window->isFullscreen && window->isMoving) {
              POINT	pt;
              GetCursorPos(&pt);
              pt.x -= window->mouse.capturedX;// + GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXEDGE);
              pt.y -= window->mouse.capturedY;// + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYEDGE);

              for(auto &[_, wHandle] : Window::windowOrder)
                SetWindowPos(wHandle, NULL, pt.x, pt.y, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
          }
        }
      } break;
      case WM_MOUSELEAVE: {
        if(window){
          window->mouse.hasFocus = false;
          log_message("WM_MOUSELEAVE", "Window \"" << window->title << "\" (" << hWnd << ") lost mouse focus");
        }
      } break;
      case WM_MOVE: {
        if(window){
          window->currentPosX = (i32)(i16)LOWORD(lParam);
          window->currentPosY = (i32)(i16)HIWORD(lParam);
          log_message("WM_MOVE", "Window \"" << window->title << "\" (" << hWnd << ") has moved to (" << window->currentPosX << ", " << window->currentPosY << ")");
        }
      } break;
      case WM_CAPTURECHANGED: {
        if(window){
          window->isMoving = (HWND)lParam == hWnd;
          if(window->isMoving){
            log_message("WM_CAPTURECHANGED", "Window \"" << window->title << "\" (" << hWnd << ") is still moving");
          }
          else{
            log_message("WM_CAPTURECHANGED", "Window \"" << window->title << "\" (" << hWnd << ") is no longer moving");
          }
        }
      } break;
      case WM_SETFOCUS: {
        if(window){
          window->hasFocus = true;
          log_message("WM_SETFOCUS", "Window \"" << window->title << "\" (" << hWnd << ") now has focus");
        }
      } break;
      case WM_KILLFOCUS: {
        if(window){
          window->hasFocus = false;
          log_message("WM_KILLFOCUS", "Window \"" << window->title << "\" (" << hWnd << ") now lost focus");
        }
      } break;
      case WM_LBUTTONDOWN: {
        if(window){
          window->mouse.leftPressed = true;
          window->mouse.leftHeld = false;
          log_message("WM_LBUTTONDOWN", "Window \"" << window->title << "\" (" << hWnd << ") captured mouse left button click");
          if(!window->isFullscreen){
            window->mouse.capturedX = LOWORD(lParam);
            window->mouse.capturedY = HIWORD(lParam);
            window->isMoving = true;
            SetCapture(hWnd);
            SetCursor(LoadCursor(NULL, IDC_SIZEALL));
          }
        }
      } break;
      case WM_LBUTTONUP: {
        if(window){
          window->mouse.leftReleased = true;
          window->mouse.leftHeld = false;
          log_message("WM_LBUTTONUP", "Window \"" << window->title << "\" (" << hWnd << ") captured mouse left button release");
          if(!window->isFullscreen)
            ReleaseCapture();
        }
      } break;
      case WM_MBUTTONDOWN: {
        if(window){
          window->mouse.middlePressed = true;
          window->mouse.middleHeld = false;
          log_message("WM_MBUTTONDOWN", "Window \"" << window->title << "\" (" << hWnd << ") captured mouse middle button click");
        }
      } break;
      case WM_MBUTTONUP: {
        if(window){
          window->mouse.middleReleased = true;
          window->mouse.middleHeld = false;
          log_message("WM_MBUTTONUP", "Window \"" << window->title << "\" (" << hWnd << ") captured mouse middle button release");
        }
      } break;
      case WM_RBUTTONDOWN: {
        if(window){
          window->mouse.rightPressed = true;
          window->mouse.rightHeld = false;
          log_message("WM_RBUTTONDOWN", "Window \"" << window->title << "\" (" << hWnd << ") captured mouse right button click");
        }
      } break;
      case WM_RBUTTONUP: {
        if(window){
          window->mouse.rightReleased = true;
          window->mouse.rightHeld = false;
          log_message("WM_RBUTTONUP", "Window \"" << window->title << "\" (" << hWnd << ") captured mouse right button release");
        }
      } break;
      //case WM_CLOSE: DestroyWindow(hWnd); break;
      //case WM_DESTROY: PostQuitMessage(0); break;
      case WM_NCDESTROY: {
        if(window){
          SelectObject(window->baseBitmapDC, window->defaultBitmap1);
          DeleteObject(window->baseBitmap);
          DeleteDC(window->baseBitmapDC);
          SelectObject(window->stretchedBitmapDC, window->defaultBitmap2);
          DeleteObject(window->stretchedBitmap);
          DeleteDC(window->stretchedBitmapDC);

          window->isRunning = false;
          Window::windowOrder.erase(window->zOrder);
          log_message("WM_NCDESTROY", "Window \"" << window->title << "\" (" << hWnd << ") was destroyed");
        }
      } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
  };

  windowClass.hInstance = GetModuleHandle(0);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpszClassName = L"sylphid";
  windowClass.hCursor = LoadCursor(0, IDC_ARROW);
  windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  windowClass.cbWndExtra = sizeof(void*);

  RegisterClass(&windowClass);
  root = std::make_unique<Window>(windowClass, sizeX, sizeY, 1, name, 0, WindowType::root);
}

void WindowManager::beginFrame() {
  if(xInputLibrary)
    updateJoystickState();
  MSG msg;
  while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
    switch(msg.message){
      case WM_KEYUP: case WM_SYSKEYDOWN: case WM_SYSKEYUP: case WM_KEYDOWN: {
        updateKeyState(msg.wParam, msg.lParam);
      } break;
      default:
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
  }
}

void WindowManager::endFrame() {
  for(i8 i = 0; i < 4; i++) {
    for(i8 j = 0; j < 32; j++) {
      if(J[i].button[j].isPressed)
        J[i].button[j].isHeld = true;
      J[i].button[j].isPressed = false;
      J[i].button[j].isReleased = false;
    }
  }

  for(i16 i = 0; i < 256; i++) {
    if(K[i].isPressed)
      K[i].isHeld = true;
    K[i].isPressed = false;
    K[i].isReleased = false;
  }

  // NOTE: I don't know what to do with modifier keys
  K[static_cast<i32>(SpecialKey::leftShift)].isHeld = false;
  K[static_cast<i32>(SpecialKey::rightShift)].isHeld = false;
  K[static_cast<i32>(SpecialKey::leftCtrl)].isHeld = false;
  K[static_cast<i32>(SpecialKey::rightCtrl)].isHeld = false;
  K[static_cast<i32>(SpecialKey::leftAlt)].isHeld = false;
  K[static_cast<i32>(SpecialKey::rightAlt)].isHeld = false;
  K[static_cast<i32>(SpecialKey::leftSuper)].isHeld = false;
  K[static_cast<i32>(SpecialKey::rightSuper)].isHeld = false;

  for(auto [_, win] : windows)
    win->endFrame();
}

void WindowManager::resetClock(){ clk.reset(); }

template<typename Time>
double WindowManager::getTotalElapsed(){ return clk.getTotalElapsed<Time>(); }

template<typename Time>
double WindowManager::getLastFrameElapsed(){ return clk.getLastFrameElapsed<Time>(); }

template<typename Time>
double WindowManager::getLastSecondElapsed(){ return clk.getLastSecondElapsed<Time>(); }

u64 WindowManager::getFrame(){ return clk.getFrame(); }
u64 WindowManager::getTotalFrames(){ return clk.getTotalFrames(); }
u64 WindowManager::getFPS(){ return clk.getFPS(); }
double WindowManager::getElapsedTime(){ return clk.getLastFrameElapsed<TimeUnit::seconds>(); }

Clock& WindowManager::tickClock(){ return clk.tick(); }

void WindowManager::updateJoystickState() {
  // TODO: Maybe investigate Winmm for joystick input (only if it's better for this use)
  for(i32 i = 0; i < 4; i++) {
    if((J[i].isOn = (xInputGetState(i, &J[i].jsState) == ERROR_SUCCESS))) {
      // Joystick is connected
      if(!J[i].button[JoystickButton::dpadUp].isHeld) {
        J[i].button[JoystickButton::dpadUp].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
        J[i].button[JoystickButton::dpadUp].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) {
        J[i].button[JoystickButton::dpadUp].isReleased = true;
        J[i].button[JoystickButton::dpadUp].isHeld = false;
      }
      if(!J[i].button[JoystickButton::dpadDown].isHeld) {
        J[i].button[JoystickButton::dpadDown].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
        J[i].button[JoystickButton::dpadDown].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) {
        J[i].button[JoystickButton::dpadDown].isReleased = true;
        J[i].button[JoystickButton::dpadDown].isHeld = false;
      }
      if(!J[i].button[JoystickButton::dpadLeft].isHeld) {
        J[i].button[JoystickButton::dpadLeft].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
        J[i].button[JoystickButton::dpadLeft].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) {
        J[i].button[JoystickButton::dpadLeft].isReleased = true;
        J[i].button[JoystickButton::dpadLeft].isHeld = false;
      }
      if(!J[i].button[JoystickButton::dpadRight].isHeld) {
        J[i].button[JoystickButton::dpadRight].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
        J[i].button[JoystickButton::dpadRight].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {
        J[i].button[JoystickButton::dpadRight].isReleased = true;
        J[i].button[JoystickButton::dpadRight].isHeld = false;
      }
      if(!J[i].button[JoystickButton::start].isHeld) {
        J[i].button[JoystickButton::start].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_START;
        J[i].button[JoystickButton::start].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
        J[i].button[JoystickButton::start].isReleased = true;
        J[i].button[JoystickButton::start].isHeld = false;
      }
      if(!J[i].button[JoystickButton::back].isHeld) {
        J[i].button[JoystickButton::back].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
        J[i].button[JoystickButton::back].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)) {
        J[i].button[JoystickButton::back].isReleased = true;
        J[i].button[JoystickButton::back].isHeld = false;
      }
      if(!J[i].button[JoystickButton::leftThumb].isHeld) {
        J[i].button[JoystickButton::leftThumb].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
        J[i].button[JoystickButton::leftThumb].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)) {
        J[i].button[JoystickButton::leftThumb].isReleased = true;
        J[i].button[JoystickButton::leftThumb].isHeld = false;
      }
      if(!J[i].button[JoystickButton::rightThumb].isHeld) {
        J[i].button[JoystickButton::rightThumb].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
        J[i].button[JoystickButton::rightThumb].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)) {
        J[i].button[JoystickButton::rightThumb].isReleased = true;
        J[i].button[JoystickButton::rightThumb].isHeld = false;
      }
      if(!J[i].button[JoystickButton::leftShoulder].isHeld) {
        J[i].button[JoystickButton::leftShoulder].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
        J[i].button[JoystickButton::leftShoulder].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
        J[i].button[JoystickButton::leftShoulder].isReleased = true;
        J[i].button[JoystickButton::leftShoulder].isHeld = false;
      }
      if(!J[i].button[JoystickButton::rightShoulder].isHeld) {
        J[i].button[JoystickButton::rightShoulder].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
        J[i].button[JoystickButton::rightShoulder].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
        J[i].button[JoystickButton::rightShoulder].isReleased = true;
        J[i].button[JoystickButton::rightShoulder].isHeld = false;
      }
      if(!J[i].button[JoystickButton::a].isHeld) {
        J[i].button[JoystickButton::a].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
        J[i].button[JoystickButton::a].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
        J[i].button[JoystickButton::a].isReleased = true;
        J[i].button[JoystickButton::a].isHeld = false;
      }
      if(!J[i].button[JoystickButton::b].isHeld) {
        J[i].button[JoystickButton::b].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_B;
        J[i].button[JoystickButton::b].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
        J[i].button[JoystickButton::b].isReleased = true;
        J[i].button[JoystickButton::b].isHeld = false;
      }
      if(!J[i].button[JoystickButton::x].isHeld) {
        J[i].button[JoystickButton::x].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
        J[i].button[JoystickButton::x].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {
        J[i].button[JoystickButton::x].isReleased = true;
        J[i].button[JoystickButton::x].isHeld = false;
      }
      if(!J[i].button[JoystickButton::y].isHeld) {
        J[i].button[JoystickButton::y].isPressed = J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
        J[i].button[JoystickButton::y].isHeld = false;
      } else if(!(J[i].jsState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)) {
        J[i].button[JoystickButton::y].isReleased = true;
        J[i].button[JoystickButton::y].isHeld = false;
      }

      J[i].axis[JoystickAxis::leftTrigger] = J[i].jsState.Gamepad.bLeftTrigger;
      J[i].axis[JoystickAxis::rightTrigger] = J[i].jsState.Gamepad.bRightTrigger;

      J[i].axis[JoystickAxis::leftThumbX] = J[i].jsState.Gamepad.sThumbLX;
      J[i].axis[JoystickAxis::leftThumbY] = J[i].jsState.Gamepad.sThumbLY;
      J[i].axis[JoystickAxis::rightThumbX] = J[i].jsState.Gamepad.sThumbRX;
      J[i].axis[JoystickAxis::rightThumbY] = J[i].jsState.Gamepad.sThumbRY;
    } else {
      // Joystick is not connected
    }
  }
}

// TODO: reinvestigate this
//void WindowManager::toggleFullscreen() {
//  if(isFullscreen) {
//    isFullscreen = false;
//    setWindowSize(previousWidth, previousHeight);
//  } else {
//    isFullscreen = true;
//    previousWidth = width;
//    previousHeight = height;
//    setWindowSize(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
//  }
//}

void WindowManager::sendRumble(i32 controller, u16 left, u16 right) {
  XINPUT_VIBRATION tmp = {left, right};
  xInputSetState(controller, &tmp);
}

void WindowManager::updateKeyState(u32 key, u32 bitfield) {
  u32 mapped_key = key;

  bool was_down = bitfield >> 30 & 1;
  bool isDown = bitfield >> 31 ^ 1;
  switch (key) {
    case VK_ESCAPE:   mapped_key = static_cast<u32>(SpecialKey::escape);      break;
    case VK_RETURN:   mapped_key = static_cast<u32>(SpecialKey::enter);       break;
    case VK_TAB:      mapped_key = static_cast<u32>(SpecialKey::tab);         break;
    case VK_BACK:     mapped_key = static_cast<u32>(SpecialKey::backspace);   break;
    case VK_INSERT:   mapped_key = static_cast<u32>(SpecialKey::insert);      break;
    case VK_DELETE:   mapped_key = static_cast<u32>(SpecialKey::deleto);      break;
    case VK_RIGHT:    mapped_key = static_cast<u32>(SpecialKey::right);       break;
    case VK_LEFT:     mapped_key = static_cast<u32>(SpecialKey::left);        break;
    case VK_DOWN:     mapped_key = static_cast<u32>(SpecialKey::down);        break;
    case VK_UP:       mapped_key = static_cast<u32>(SpecialKey::up);          break;
    case VK_PRIOR:    mapped_key = static_cast<u32>(SpecialKey::pageUp);      break;
    case VK_NEXT:     mapped_key = static_cast<u32>(SpecialKey::pageDown);    break;
    case VK_HOME:     mapped_key = static_cast<u32>(SpecialKey::home);        break;
    case VK_END:      mapped_key = static_cast<u32>(SpecialKey::end);         break;
    case VK_CAPITAL:  mapped_key = static_cast<u32>(SpecialKey::capslock);    break;
    case VK_SCROLL:   mapped_key = static_cast<u32>(SpecialKey::scrollLock);  break;
    case VK_NUMLOCK:  mapped_key = static_cast<u32>(SpecialKey::numLock);     break;
    case VK_SNAPSHOT: mapped_key = static_cast<u32>(SpecialKey::printScreen); break;
    case VK_PAUSE:    mapped_key = static_cast<u32>(SpecialKey::pauseBreak);  break;
    case VK_F1:       mapped_key = static_cast<u32>(SpecialKey::f1);          break;
    case VK_F2:       mapped_key = static_cast<u32>(SpecialKey::f2);          break;
    case VK_F3:       mapped_key = static_cast<u32>(SpecialKey::f3);          break;
    case VK_F4:       mapped_key = static_cast<u32>(SpecialKey::f4);          break;
    case VK_F5:       mapped_key = static_cast<u32>(SpecialKey::f5);          break;
    case VK_F6:       mapped_key = static_cast<u32>(SpecialKey::f6);          break;
    case VK_F7:       mapped_key = static_cast<u32>(SpecialKey::f7);          break;
    case VK_F8:       mapped_key = static_cast<u32>(SpecialKey::f8);          break;
    case VK_F9:       mapped_key = static_cast<u32>(SpecialKey::f9);          break;
    case VK_F10:      mapped_key = static_cast<u32>(SpecialKey::f10);         break;
    case VK_F11:      mapped_key = static_cast<u32>(SpecialKey::f11);         break;
    case VK_F12:      mapped_key = static_cast<u32>(SpecialKey::f12);         break;
    case VK_NUMPAD0:  mapped_key = static_cast<u32>(SpecialKey::numpad0);     break;
    case VK_NUMPAD1:  mapped_key = static_cast<u32>(SpecialKey::numpad1);     break;
    case VK_NUMPAD2:  mapped_key = static_cast<u32>(SpecialKey::numpad2);     break;
    case VK_NUMPAD3:  mapped_key = static_cast<u32>(SpecialKey::numpad3);     break;
    case VK_NUMPAD4:  mapped_key = static_cast<u32>(SpecialKey::numpad4);     break;
    case VK_NUMPAD5:  mapped_key = static_cast<u32>(SpecialKey::numpad5);     break;
    case VK_NUMPAD6:  mapped_key = static_cast<u32>(SpecialKey::numpad6);     break;
    case VK_NUMPAD7:  mapped_key = static_cast<u32>(SpecialKey::numpad7);     break;
    case VK_NUMPAD8:  mapped_key = static_cast<u32>(SpecialKey::numpad8);     break;
    case VK_NUMPAD9:  mapped_key = static_cast<u32>(SpecialKey::numpad9);     break;
    case VK_DECIMAL:  mapped_key = static_cast<u32>(SpecialKey::decimal);     break;
    case VK_DIVIDE:   mapped_key = static_cast<u32>(SpecialKey::divide);      break;
    case VK_MULTIPLY: mapped_key = static_cast<u32>(SpecialKey::multiply);    break;
    case VK_SUBTRACT: mapped_key = static_cast<u32>(SpecialKey::subtract);    break;
    case VK_ADD:      mapped_key = static_cast<u32>(SpecialKey::add);         break;
    // TODO: fix special VK_KEYS with right variants (or leave as is)
    case VK_SHIFT:   
    case VK_LSHIFT:   mapped_key = static_cast<u32>(SpecialKey::leftShift);   break;
    case VK_CONTROL: 
    case VK_LCONTROL: mapped_key = static_cast<u32>(SpecialKey::leftCtrl);    break;
    case VK_MENU:    
    case VK_LMENU:    mapped_key = static_cast<u32>(SpecialKey::leftAlt);     break;
    case VK_LWIN:     mapped_key = static_cast<u32>(SpecialKey::leftSuper);   break;
    case VK_RSHIFT:   mapped_key = static_cast<u32>(SpecialKey::rightShift);  break;
    case VK_RCONTROL: mapped_key = static_cast<u32>(SpecialKey::rightCtrl);   break;
    case VK_RMENU:    mapped_key = static_cast<u32>(SpecialKey::rightAlt);    break;
    case VK_RWIN:     mapped_key = static_cast<u32>(SpecialKey::rightSuper);  break;
  }
  if(mapped_key >= 'A' && mapped_key <= 'Z') {
    K[mapped_key + 32].isPressed = isDown;
    K[mapped_key + 32].isHeld = false;
    K[mapped_key + 32].isReleased = (was_down) & (!isDown);
  }
  K[mapped_key].isPressed = isDown;
  K[mapped_key].isHeld = false;
  K[mapped_key].isReleased = (was_down) & (!isDown);
  //if(K[kF4].isPressed && ((bitfield >> 29) & 1)) // Check Alt-F4
  //  isRunning = false;
}

void WindowManager::draw(){
  root->draw();
}

void WindowManager::drawWindows(){
  for(const auto& [_, win] : windowsDrawingOrder)
    win->draw();
}

void WindowManager::drawAll(){
  draw();
  drawWindows();
}

void WindowManager::removeWindow(std::wstring windowKey){
  DestroyWindow(windows[windowKey]->hWnd);
  windowsDrawingOrder.erase(windows[windowKey]->zOrder);
  windows.erase(windowKey);
}

void WindowManager::removeAllWindows(){
  for(auto [_, win] : windows){
    DestroyWindow(win->hWnd);
    log_debug("Destroyed window \"" << win->title << "\" (" << win->hWnd << ")");
  }

  windowsDrawingOrder.clear();
  windows.clear();
}

void WindowManager::createWindow(std::wstring windowTitle, i32 windowZOrder, i32 windowSizeX, i32 windowSizeY, i32 scale){
  std::shared_ptr<Window> newWindow = std::make_shared<Window>(windowClass, windowSizeX, windowSizeY, scale, windowTitle, windowZOrder);
  windows[windowTitle] = newWindow;
  windowsDrawingOrder[windowZOrder] = newWindow;

  // Resort
  for(auto &[_, wHandle] : Window::windowOrder)
    SetWindowPos(wHandle, HWND_TOP, -1, -1, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

const Key& WindowManager::operator()(SpecialKey spKey) const{
  return K[static_cast<i32>(spKey)];
}

const Key& WindowManager::operator()(char key) const{
  return K[key];
}

const std::shared_ptr<Window> WindowManager::operator[](std::wstring windowKey){
  return windows[windowKey];
}

bool WindowManager::shouldRun(){
  endFrame();
  tickClock();
  beginFrame();
  drawAll();
  return root->isRunning;
}
