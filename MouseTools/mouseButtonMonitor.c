#define WIN32_LEAN_AND_MEAN
#include "mouseButtonMonitor.h"

static const int mouseButtonVirtualKeys[MOUSE_BUTTON_COUNT] = {
    VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2};

BOOL mouseButtonMonitorIsDown(MouseButton button) {
  if (button < 0 || button >= MOUSE_BUTTON_COUNT) {
    return FALSE;
  }

  return (GetAsyncKeyState(mouseButtonVirtualKeys[button]) & 0x8000) != 0;
}

DWORD mouseButtonMonitorGetState(void) {
  DWORD state = 0;

  for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
    if (mouseButtonMonitorIsDown((MouseButton)i)) {
      state |= (DWORD)(1u << i);
    }
  }

  return state;
}
