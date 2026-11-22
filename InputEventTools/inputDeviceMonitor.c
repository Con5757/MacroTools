#include "inputDeviceMonitor.h"
#include <string.h>

BOOL inputDeviceMonitorInitialize(InputDeviceMonitor *monitor,
                                  HWND targetWindow, BOOL receiveInBackground) {
  if (monitor == NULL || targetWindow == NULL)
    return FALSE;
  memset(monitor, 0, sizeof(*monitor));
  USHORT flags = RIDEV_DEVNOTIFY;
  if (receiveInBackground)
    flags = (USHORT)(flags | RIDEV_INPUTSINK);
  RAWINPUTDEVICE devices[2] = {{0x01, 0x02, flags, targetWindow},
                               {0x01, 0x06, flags, targetWindow}};
  if (!RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE)))
    return FALSE;
  monitor->targetWindow = targetWindow;
  monitor->registered = TRUE;
  return TRUE;
}
BOOL inputDeviceMonitorHandleMessage(InputDeviceMonitor *monitor, UINT message,
                                     WPARAM wParam, LPARAM lParam,
                                     InputEvent *event) {
  if (monitor == NULL || !monitor->registered || event == NULL ||
      message != WM_INPUT_DEVICE_CHANGE)
    return FALSE;
  memset(event, 0, sizeof(*event));
  event->timestampMs = GetTickCount64();
  event->type = wParam == GIDC_ARRIVAL ? INPUT_EVENT_DEVICE_ARRIVAL
                                       : INPUT_EVENT_DEVICE_REMOVAL;
  event->deviceHandle = (HANDLE)lParam;
  event->source = INPUT_EVENT_SOURCE_UNKNOWN;
  return TRUE;
}
void inputDeviceMonitorShutdown(InputDeviceMonitor *monitor) {
  if (monitor == NULL)
    return;
  if (monitor->registered) {
    RAWINPUTDEVICE devices[2] = {{0x01, 0x02, RIDEV_REMOVE, NULL},
                                 {0x01, 0x06, RIDEV_REMOVE, NULL}};
    RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE));
  }
  memset(monitor, 0, sizeof(*monitor));
}