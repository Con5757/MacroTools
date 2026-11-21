#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define KEY_STATE_MONITOR_KEY_COUNT 256

typedef struct {
  BYTE keyDown[KEY_STATE_MONITOR_KEY_COUNT];
  ULONGLONG timestampMs;
} KeyStateMonitorData;

BOOL keyStateMonitorUpdate(KeyStateMonitorData *data);
BOOL keyStateMonitorIsDown(const KeyStateMonitorData *data, WORD virtualKey);