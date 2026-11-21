#define WIN32_LEAN_AND_MEAN
#include "keyStateMonitor.h"

BOOL keyStateMonitorUpdate(KeyStateMonitorData *data) {
  if (data == NULL) {
    return FALSE;
  }

  for (unsigned int key = 0; key < KEY_STATE_MONITOR_KEY_COUNT; ++key) {
    data->keyDown[key] = (GetAsyncKeyState((int)key) & 0x8000) != 0;
  }

  data->timestampMs = GetTickCount64();
  return TRUE;
}

BOOL keyStateMonitorIsDown(const KeyStateMonitorData *data, WORD virtualKey) {
  if (data == NULL || virtualKey >= KEY_STATE_MONITOR_KEY_COUNT) {
    return FALSE;
  }

  return data->keyDown[virtualKey] != 0;
}
