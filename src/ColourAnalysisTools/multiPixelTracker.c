#include "multiPixelTracker.h"

#include <stdlib.h>

BOOL multiPixelTrackerInitialize(MultiPixelTracker *tracker,
                                 const POINT *coordinates, size_t count,
                                 DWORD intervalMs) {
  if (tracker == NULL || coordinates == NULL || count == 0 ||
      count > SIZE_MAX / sizeof(MultiPixelTrackedPoint) || intervalMs == 0 ||
      intervalMs > 60000u) {
    return FALSE;
  }

  ZeroMemory(tracker, sizeof(*tracker));
  tracker->points =
      (MultiPixelTrackedPoint *)calloc(count, sizeof(MultiPixelTrackedPoint));
  if (tracker->points == NULL) {
    return FALSE;
  }

  size_t i;
  for (i = 0; i < count; ++i) {
    tracker->points[i].x = coordinates[i].x;
    tracker->points[i].y = coordinates[i].y;
  }

  tracker->count = count;
  tracker->intervalMs = intervalMs;
  return TRUE;
}

BOOL multiPixelTrackerUpdate(MultiPixelTracker *tracker,
                             ULONGLONG *timestampMs) {
  if (tracker == NULL || tracker->points == NULL) {
    return FALSE;
  }

  ULONGLONG now = GetTickCount64();
  if (tracker->hasSample &&
      now - tracker->lastSampleTime < tracker->intervalMs) {
    return FALSE;
  }

  size_t i;
  for (i = 0; i < tracker->count; ++i) {
    PixelColor color;
    if (pixelReadColorAt(tracker->points[i].x, tracker->points[i].y, &color)) {
      tracker->points[i].color = color;
      tracker->points[i].valid = TRUE;
    } else {
      tracker->points[i].valid = FALSE;
    }
  }

  tracker->lastSampleTime = now;
  tracker->hasSample = TRUE;
  if (timestampMs != NULL)
    *timestampMs = now;
  return TRUE;
}

const MultiPixelTrackedPoint *
multiPixelTrackerGetPoints(const MultiPixelTracker *tracker, size_t *count) {
  if (count != NULL)
    *count = tracker != NULL ? tracker->count : 0;
  return tracker != NULL ? tracker->points : NULL;
}

void multiPixelTrackerDestroy(MultiPixelTracker *tracker) {
  if (tracker == NULL)
    return;
  free(tracker->points);
  ZeroMemory(tracker, sizeof(*tracker));
}