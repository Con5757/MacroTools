#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_EVENT_MONITOR_MAX_EVENTS 8

typedef struct InputEventMonitor {
  HWND targetWindow;
  BOOL keyboardRegistered;
  BOOL mouseRegistered;
} InputEventMonitor;

BOOL inputEventMonitorInitialize(
    InputEventMonitor *monitor, HWND targetWindow, BOOL captureKeyboard,
    BOOL captureMouse,
    BOOL receiveInBackground); // targetWindow has to belong to the host
                               // applications message loop
size_t inputEventMonitorHandleMessage(
    InputEventMonitor *monitor, UINT message, WPARAM wParam, LPARAM lParam,
    InputEvent *events,
    size_t capacity); // call from wndproc, returns num of events
void inputEventMonitorShutdown(InputEventMonitor *monitor);

#ifdef __cplusplus
}
#endif