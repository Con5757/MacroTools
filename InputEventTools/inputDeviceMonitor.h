#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputDeviceMonitor {
  HWND targetWindow;
  BOOL registered;
} InputDeviceMonitor;

BOOL inputDeviceMonitorInitialize(InputDeviceMonitor *monitor,
                                  HWND targetWindow, BOOL receiveInBackground);
BOOL inputDeviceMonitorHandleMessage(InputDeviceMonitor *monitor, UINT message,
                                     WPARAM wParam, LPARAM lParam,
                                     InputEvent *event);
void inputDeviceMonitorShutdown(InputDeviceMonitor *monitor);

#ifdef __cplusplus
}
#endif