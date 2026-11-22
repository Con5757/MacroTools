#include "inputEventMonitor.h"
#include <stdlib.h>
#include <string.h>

static size_t appendEvent(InputEvent *events, size_t capacity, size_t count,
                          const InputEvent *event) {
  if (count < capacity)
    events[count] = *event;
  return count + 1;
}
BOOL inputEventMonitorInitialize(InputEventMonitor *monitor, HWND targetWindow,
                                 BOOL captureKeyboard, BOOL captureMouse,
                                 BOOL receiveInBackground) {
  if (monitor == NULL || targetWindow == NULL ||
      (!captureKeyboard && !captureMouse))
    return FALSE;
  memset(monitor, 0, sizeof(*monitor));
  RAWINPUTDEVICE devices[2];
  UINT count = 0;
  USHORT flags = RIDEV_DEVNOTIFY;
  if (receiveInBackground)
    flags = (USHORT)(flags | RIDEV_INPUTSINK);
  if (captureMouse) {
    devices[count].usUsagePage = 0x01;
    devices[count].usUsage = 0x02;
    devices[count].dwFlags = flags;
    devices[count].hwndTarget = targetWindow;
    ++count;
  }
  if (captureKeyboard) {
    devices[count].usUsagePage = 0x01;
    devices[count].usUsage = 0x06;
    devices[count].dwFlags = flags;
    devices[count].hwndTarget = targetWindow;
    ++count;
  }
  if (!RegisterRawInputDevices(devices, count, sizeof(RAWINPUTDEVICE)))
    return FALSE;
  monitor->targetWindow = targetWindow;
  monitor->keyboardRegistered = captureKeyboard;
  monitor->mouseRegistered = captureMouse;
  return TRUE;
}
size_t inputEventMonitorHandleMessage(InputEventMonitor *monitor, UINT message,
                                      WPARAM wParam, LPARAM lParam,
                                      InputEvent *events, size_t capacity) {
  if (monitor == NULL || events == NULL || capacity == 0)
    return 0;
  if (message == WM_INPUT_DEVICE_CHANGE) {
    InputEvent event;
    memset(&event, 0, sizeof(event));
    event.timestampMs = GetTickCount64();
    event.messageTime = (DWORD)GetMessageTime();
    event.type = (wParam == GIDC_ARRIVAL) ? INPUT_EVENT_DEVICE_ARRIVAL
                                          : INPUT_EVENT_DEVICE_REMOVAL;
    event.deviceHandle = (HANDLE)lParam;
    event.source = INPUT_EVENT_SOURCE_UNKNOWN;
    events[0] = event;
    return 1;
  }
  if (message != WM_INPUT)
    return 0;

  UINT bytes = 0;
  if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bytes,
                      sizeof(RAWINPUTHEADER)) == (UINT)-1 ||
      bytes < sizeof(RAWINPUT))
    return 0;
  BYTE *storage = (BYTE *)malloc(bytes);
  if (storage == NULL)
    return 0;
  UINT readBytes = bytes;
  UINT result = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, storage,
                                &readBytes, sizeof(RAWINPUTHEADER));
  if (result == (UINT)-1 || readBytes < sizeof(RAWINPUTHEADER)) {
    free(storage);
    return 0;
  }
  RAWINPUT *raw = (RAWINPUT *)storage;
  size_t count = 0;
  ULONGLONG now = GetTickCount64();

  if (raw->header.dwType == RIM_TYPEKEYBOARD && monitor->keyboardRegistered) {
    InputEvent event;
    memset(&event, 0, sizeof(event));
    event.timestampMs = now;
    event.messageTime = (DWORD)GetMessageTime();
    event.type = (raw->data.keyboard.Flags & RI_KEY_BREAK)
                     ? INPUT_EVENT_KEY_UP
                     : INPUT_EVENT_KEY_DOWN;
    event.deviceHandle = raw->header.hDevice;
    event.source = inputEventClassifySource(raw->header.hDevice, FALSE);
    event.virtualKey = raw->data.keyboard.VKey;
    event.scanCode = raw->data.keyboard.MakeCode;
    event.rawKeyboardFlags = raw->data.keyboard.Flags;
    event.extraInfo = raw->data.keyboard.ExtraInformation;
    count = appendEvent(events, capacity, count, &event);
  } else if (raw->header.dwType == RIM_TYPEMOUSE && monitor->mouseRegistered) {
    const RAWMOUSE *mouse = &raw->data.mouse;
    if (mouse->lLastX != 0 || mouse->lLastY != 0 ||
        (mouse->usFlags & MOUSE_MOVE_ABSOLUTE) != 0) {
      InputEvent event;
      memset(&event, 0, sizeof(event));
      event.timestampMs = now;
      event.messageTime = (DWORD)GetMessageTime();
      event.type = INPUT_EVENT_MOUSE_MOVE;
      event.deviceHandle = raw->header.hDevice;
      event.source = inputEventClassifySource(raw->header.hDevice, FALSE);
      event.rawMouseFlags = mouse->usFlags;
      event.x = mouse->lLastX;
      event.y = mouse->lLastY;
      event.extraInfo = mouse->ulExtraInformation;
      count = appendEvent(events, capacity, count, &event);
    }
    struct ButtonMap {
      USHORT down;
      USHORT up;
      UINT button;
    } maps[] = {{RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_LEFT_BUTTON_UP, 1},
                {RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_UP, 2},
                {RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP, 3},
                {RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_4_UP, 4},
                {RI_MOUSE_BUTTON_5_DOWN, RI_MOUSE_BUTTON_5_UP, 5}};
    for (size_t i = 0; i < sizeof(maps) / sizeof(maps[0]); ++i) {
      if ((mouse->usButtonFlags & maps[i].down) != 0 ||
          (mouse->usButtonFlags & maps[i].up) != 0) {
        InputEvent event;
        memset(&event, 0, sizeof(event));
        event.timestampMs = now;
        event.messageTime = (DWORD)GetMessageTime();
        event.type = (mouse->usButtonFlags & maps[i].down)
                         ? INPUT_EVENT_MOUSE_BUTTON_DOWN
                         : INPUT_EVENT_MOUSE_BUTTON_UP;
        event.deviceHandle = raw->header.hDevice;
        event.source = inputEventClassifySource(raw->header.hDevice, FALSE);
        event.mouseButton = maps[i].button;
        event.rawMouseFlags = mouse->usFlags;
        event.rawButtonFlags = mouse->usButtonFlags;
        event.rawButtonData = mouse->usButtonData;
        event.extraInfo = mouse->ulExtraInformation;
        count = appendEvent(events, capacity, count, &event);
      }
    }
    if ((mouse->usButtonFlags & (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL)) != 0) {
      InputEvent event;
      memset(&event, 0, sizeof(event));
      event.timestampMs = now;
      event.messageTime = (DWORD)GetMessageTime();
      event.type = INPUT_EVENT_MOUSE_WHEEL;
      event.deviceHandle = raw->header.hDevice;
      event.source = inputEventClassifySource(raw->header.hDevice, FALSE);
      event.rawMouseFlags = mouse->usFlags;
      event.rawButtonFlags = mouse->usButtonFlags;
      event.rawButtonData = mouse->usButtonData;
      event.wheelDelta = (SHORT)mouse->usButtonData;
      event.extraInfo = mouse->ulExtraInformation;
      count = appendEvent(events, capacity, count, &event);
    }
  }
  free(storage);
  return count > capacity ? capacity : count;
}
void inputEventMonitorShutdown(InputEventMonitor *monitor) {
  if (monitor == NULL)
    return;
  RAWINPUTDEVICE devices[2];
  UINT count = 0;
  if (monitor->mouseRegistered) {
    devices[count].usUsagePage = 0x01;
    devices[count].usUsage = 0x02;
    devices[count].dwFlags = RIDEV_REMOVE;
    devices[count].hwndTarget = NULL;
    ++count;
  }
  if (monitor->keyboardRegistered) {
    devices[count].usUsagePage = 0x01;
    devices[count].usUsage = 0x06;
    devices[count].dwFlags = RIDEV_REMOVE;
    devices[count].hwndTarget = NULL;
    ++count;
  }
  if (count != 0)
    RegisterRawInputDevices(devices, count, sizeof(RAWINPUTDEVICE));
  memset(monitor, 0, sizeof(*monitor));
}