#define WIN32_LEAN_AND_MEAN
#include "mouseDistanceTracker.h"

#include <math.h>

void mouseDistanceTrackerInitialize(MouseDistanceTracker *tracker) {
  if (tracker != NULL) {
    ZeroMemory(tracker, sizeof(*tracker));
  }
}

BOOL mouseDistanceTrackerUpdate(MouseDistanceTracker *tracker,
                                double *segmentDistancePixels) {
  if (tracker == NULL) {
    return FALSE;
  }

  POINT current;
  if (!GetCursorPos(&current)) {
    return FALSE;
  }

  double distance = 0.0;
  if (tracker->initialized) {
    LONG dx = current.x - tracker->previousPosition.x;
    LONG dy = current.y - tracker->previousPosition.y;
    distance = sqrt((double)dx * dx + (double)dy * dy);
    tracker->totalDistancePixels += distance;
  }

  tracker->previousPosition = current;
  tracker->lastSegmentDistancePixels = distance;
  tracker->sampleCount++;
  tracker->initialized = TRUE;

  if (segmentDistancePixels != NULL) {
    *segmentDistancePixels = distance;
  }
  return TRUE;
}

void mouseDistanceTrackerReset(MouseDistanceTracker *tracker) {
  mouseDistanceTrackerInitialize(tracker);
}

double mouseDistanceTrackerGetTotal(const MouseDistanceTracker *tracker) {
  return tracker != NULL ? tracker->totalDistancePixels : 0.0;
}
