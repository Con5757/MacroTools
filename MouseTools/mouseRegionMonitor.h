#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

typedef enum {
  MOUSE_REGION_NO_CHANGE = 0,
  MOUSE_REGION_ENTERED,
  MOUSE_REGION_LEFT
} MouseRegionEvent;

typedef struct {
  RECT region;
  BOOL isInside;
  BOOL initialized;
} MouseRegionMonitor;

BOOL mouseRegionMonitorInitialize(MouseRegionMonitor *monitor, RECT region);
BOOL mouseRegionMonitorUpdate(MouseRegionMonitor *monitor,
                              MouseRegionEvent *event, POINT *cursorPosition);
BOOL mouseRegionMonitorSetRegion(MouseRegionMonitor *monitor, RECT region);
BOOL mouseRegionMonitorIsInside(const MouseRegionMonitor *monitor);
