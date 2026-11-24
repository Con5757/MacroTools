#include "pixelTracker.h"

BOOL pixelTrackerInitialize(PixelTracker *tracker, int x, int y,
                            DWORD intervalMs) {
  if (tracker == NULL || intervalMs == 0 || intervalMs > 60000u) {
    return FALSE;
  }

  ZeroMemory(tracker, sizeof(*tracker));
  tracker->x = x;
  tracker->y = y;
  tracker->intervalMs = intervalMs;
  return TRUE;
}

BOOL pixelTrackerUpdate(PixelTracker *tracker, PixelColor *color,
                        ULONGLONG *timestampMs) {
  if (tracker == NULL || color == NULL) {
    return FALSE;
  }

  ULONGLONG now = GetTickCount64();
  if (tracker->hasSample &&
      now - tracker->lastSampleTime < tracker->intervalMs) {
    return FALSE;
  }

  PixelColor current;
  if (!pixelReadColorAt(tracker->x, tracker->y, &current)) {
    return FALSE;
  }

  tracker->lastColor = current;
  tracker->lastSampleTime = now;
  tracker->hasSample = TRUE;
  *color = current;

  if (timestampMs != NULL) {
    *timestampMs = now;
  }
  return TRUE;
}