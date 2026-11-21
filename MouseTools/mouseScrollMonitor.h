#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

typedef enum {
  MOUSE_SCROLL_VERTICAL = 0,
  MOUSE_SCROLL_HORIZONTAL = 1
} MouseScrollAxis;

typedef struct {
  SHORT lastVerticalDelta;
  SHORT lastHorizontalDelta;
  LONGLONG totalVerticalDelta;
  LONGLONG totalHorizontalDelta;
  ULONGLONG verticalEventCount;
  ULONGLONG horizontalEventCount;
  ULONGLONG lastEventTimeMs;
} MouseScrollMonitor;

void mouseScrollMonitorInitialize(MouseScrollMonitor *monitor);
BOOL mouseScrollMonitorHandleMessage(MouseScrollMonitor *monitor, UINT message,
                                     WPARAM wParam);
void mouseScrollMonitorReset(MouseScrollMonitor *monitor);