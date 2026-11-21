#define WIN32_LEAN_AND_MEAN
#include "mouseRegionMonitor.h"

BOOL mouseRegionMonitorInitialize(MouseRegionMonitor *monitor, RECT region) {
  if (monitor == NULL || region.right <= region.left ||
      region.bottom <= region.top) {
    return FALSE;
  }

  ZeroMemory(monitor, sizeof(*monitor));
  monitor->region = region;
  return TRUE;
}

BOOL mouseRegionMonitorUpdate(MouseRegionMonitor *monitor,
                              MouseRegionEvent *event, POINT *cursorPosition) {
  if (monitor == NULL || event == NULL) {
    return FALSE;
  }

  POINT cursor;
  if (!GetCursorPos(&cursor)) {
    return FALSE;
  }

  BOOL inside = PtInRect(&monitor->region, cursor);
  *event = MOUSE_REGION_NO_CHANGE;

  if (monitor->initialized && inside != monitor->isInside) {
    *event = inside ? MOUSE_REGION_ENTERED : MOUSE_REGION_LEFT;
  }

  monitor->isInside = inside;
  monitor->initialized = TRUE;

  if (cursorPosition != NULL) {
    *cursorPosition = cursor;
  }
  return TRUE;
}

BOOL mouseRegionMonitorSetRegion(MouseRegionMonitor *monitor, RECT region) {
  if (monitor == NULL || region.right <= region.left ||
      region.bottom <= region.top) {
    return FALSE;
  }

  monitor->region = region;
  monitor->isInside = FALSE;
  monitor->initialized = FALSE;
  return TRUE;
}

BOOL mouseRegionMonitorIsInside(const MouseRegionMonitor *monitor) {
  return monitor != NULL && monitor->initialized && monitor->isInside;
}
