#define WIN32_LEAN_AND_MEAN
#include "mouseBoundaryDetector.h"

#include <stdlib.h>

static MouseBoundaryEdge mouseBoundaryNearestEdge(POINT point, RECT rect) {
  LONG distances[4] = {
      labs((long)(point.x - rect.left)), labs((long)(point.y - rect.top)),
      labs((long)(point.x - rect.right)), labs((long)(point.y - rect.bottom))};
  int nearest = 0;
  for (int i = 1; i < 4; ++i) {
    if (distances[i] < distances[nearest])
      nearest = i;
  }
  return (MouseBoundaryEdge)(MOUSE_BOUNDARY_EDGE_LEFT + nearest);
}

BOOL mouseBoundaryDetectorInitialize(MouseBoundaryDetector *detector,
                                     RECT bounds) {
  if (detector == NULL || bounds.right <= bounds.left ||
      bounds.bottom <= bounds.top) {
    return FALSE;
  }

  ZeroMemory(detector, sizeof(*detector));
  detector->bounds = bounds;
  return TRUE;
}

BOOL mouseBoundaryDetectorUpdate(MouseBoundaryDetector *detector,
                                 MouseBoundaryEvent *event) {
  if (detector == NULL || event == NULL) {
    return FALSE;
  }

  POINT current;
  if (!GetCursorPos(&current)) {
    return FALSE;
  }

  ZeroMemory(event, sizeof(*event));
  event->position = current;
  event->isInside = PtInRect(&detector->bounds, current);

  if (detector->initialized && event->isInside != detector->wasInside) {
    event->type =
        event->isInside ? MOUSE_BOUNDARY_ENTERED : MOUSE_BOUNDARY_EXITED;
    event->edge = mouseBoundaryNearestEdge(
        event->isInside ? detector->previousPosition : current,
        detector->bounds);
  }

  detector->previousPosition = current;
  detector->wasInside = event->isInside;
  detector->initialized = TRUE;
  return TRUE;
}

void mouseBoundaryDetectorSetBounds(MouseBoundaryDetector *detector,
                                    RECT bounds) {
  if (detector == NULL || bounds.right <= bounds.left ||
      bounds.bottom <= bounds.top) {
    return;
  }

  detector->bounds = bounds;
  detector->initialized = FALSE;
}
