#include "engine/input.hpp"

HMODULE xInputLibrary = NULL;
DWORD (WINAPI *xInputGetState)(DWORD, XINPUT_STATE*) = NULL;
DWORD (WINAPI *xInputSetState)(DWORD, XINPUT_VIBRATION*) = NULL;
