#define WIN32_LEAN_AND_MEAN
#include "keyHoldDurationTracker.h"

BOOL keyHoldDurationTrackerInitialize(KeyHoldDurationTracker *tracker) {
  if (tracker == NULL)
    return FALSE;
  ZeroMemory(tracker, sizeof(*tracker));
  tracker->initialized = TRUE;
  return TRUE;
}

BOOL keyHoldDurationTrackerUpdate(KeyHoldDurationTracker *tracker) {
  if (tracker == NULL || !tracker->initialized)
    return FALSE;

  ULONGLONG now = GetTickCount64();
  for (unsigned int key = 1; key < 256; ++key) {
    BOOL down = (GetAsyncKeyState((int)key) & 0x8000) != 0;

    if (down && !tracker->isDown[key]) {
      tracker->isDown[key] = 1;
      tracker->pressStartedAt[key] = now;
    } else if (!down && tracker->isDown[key]) {
      tracker->lastDurationMs[key] = now - tracker->pressStartedAt[key];
      tracker->isDown[key] = 0;
      tracker->pressStartedAt[key] = 0;
    }
  }
  return TRUE;
}

BOOL keyHoldDurationTrackerGetDuration(const KeyHoldDurationTracker *tracker,
                                       WORD virtualKey, BOOL *isDown,
                                       ULONGLONG *durationMs) {
  if (tracker == NULL || !tracker->initialized || virtualKey == 0 ||
      virtualKey > 255 || isDown == NULL || durationMs == NULL) {
    return FALSE;
  }

  *isDown = tracker->isDown[virtualKey] != 0;
  *durationMs = *isDown ? GetTickCount64() - tracker->pressStartedAt[virtualKey]
                        : tracker->lastDurationMs[virtualKey];
  return TRUE;
}
