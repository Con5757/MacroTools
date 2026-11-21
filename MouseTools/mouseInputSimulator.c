#define WIN32_LEAN_AND_MEAN
#include "mouseInputSimulator.h"

static BOOL mouseInputSimulatorSendMouse(DWORD flags, DWORD mouseData, LONG dx,
                                         LONG dy) {
  INPUT input;
  ZeroMemory(&input, sizeof(input));
  input.type = INPUT_MOUSE;
  input.mi.dx = dx;
  input.mi.dy = dy;
  input.mi.mouseData = mouseData;
  input.mi.dwFlags = flags;
  return SendInput(1, &input, sizeof(input)) == 1;
}

BOOL mouseInputSimulatorMoveTo(POINT position) {
  int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

  if (width <= 1 || height <= 1) {
    return FALSE;
  }

  LONG normalizedX =
      (LONG)(((double)(position.x - left) * 65535.0) / (double)(width - 1) +
             0.5);
  LONG normalizedY =
      (LONG)(((double)(position.y - top) * 65535.0) / (double)(height - 1) +
             0.5);

  if (normalizedX < 0)
    normalizedX = 0;
  if (normalizedX > 65535)
    normalizedX = 65535;
  if (normalizedY < 0)
    normalizedY = 0;
  if (normalizedY > 65535)
    normalizedY = 65535;

  return mouseInputSimulatorSendMouse(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE |
                                          MOUSEEVENTF_VIRTUALDESK,
                                      0, normalizedX, normalizedY);
}

BOOL mouseInputSimulatorMoveRelative(LONG deltaX, LONG deltaY) {
  return mouseInputSimulatorSendMouse(MOUSEEVENTF_MOVE, 0, deltaX, deltaY);
}

BOOL mouseInputSimulatorSetButton(MouseButton button, BOOL down) {
  DWORD flags;
  DWORD data = 0;

  switch (button) {
  case MOUSE_BUTTON_LEFT:
    flags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    break;
  case MOUSE_BUTTON_RIGHT:
    flags = down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
    break;
  case MOUSE_BUTTON_MIDDLE:
    flags = down ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
    break;
  case MOUSE_BUTTON_X1:
    flags = down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
    data = XBUTTON1;
    break;
  case MOUSE_BUTTON_X2:
    flags = down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
    data = XBUTTON2;
    break;
  default:
    return FALSE;
  }

  return mouseInputSimulatorSendMouse(flags, data, 0, 0);
}

BOOL mouseInputSimulatorScrollVertical(SHORT wheelDelta) {
  return mouseInputSimulatorSendMouse(MOUSEEVENTF_WHEEL,
                                      (DWORD)(LONG)wheelDelta, 0, 0);
}

BOOL mouseInputSimulatorScrollHorizontal(SHORT wheelDelta) {
  return mouseInputSimulatorSendMouse(MOUSEEVENTF_HWHEEL,
                                      (DWORD)(LONG)wheelDelta, 0, 0);
}
