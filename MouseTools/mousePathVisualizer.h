#pragma once

#include "mouseRecorder.h"
#include <Windows.h>
#include <stddef.h>

typedef struct {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
  BOOL valid;
} MousePathBounds;

BOOL mousePathVisualizerGetBounds(const MouseRecorderSample *samples,
                                  size_t sampleCount, MousePathBounds *bounds);
BOOL mousePathVisualizerMapPoint(POINT desktopPoint,
                                 const MousePathBounds *pathBounds,
                                 RECT targetBounds, POINT *targetPoint);