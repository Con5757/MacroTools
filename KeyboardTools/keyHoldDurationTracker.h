#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

typedef struct {
  ULONGLONG pressStartedAt[256];
  ULONGLONG lastDurationMs[256];
  BYTE isDown[256];
  BOOL initialized;
} KeyHoldDurationTracker;

BOOL keyHoldDurationTrackerInitialize(KeyHoldDurationTracker *tracker);
BOOL keyHoldDurationTrackerUpdate(KeyHoldDurationTracker *tracker);
BOOL keyHoldDurationTrackerGetDuration(const KeyHoldDurationTracker *tracker,
                                       WORD virtualKey, BOOL *isDown,
                                       ULONGLONG *durationMs);
