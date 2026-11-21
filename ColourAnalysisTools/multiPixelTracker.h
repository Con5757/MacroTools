#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  int x;
  int y;
  PixelColor color;
  BOOL valid;
} MultiPixelTrackedPoint;

typedef struct {
  MultiPixelTrackedPoint *points;
  size_t count;
  DWORD intervalMs;
  ULONGLONG lastSampleTime;
  BOOL hasSample;
} MultiPixelTracker;

BOOL multiPixelTrackerInitialize(MultiPixelTracker *tracker,
                                 const POINT *coordinates, size_t count,
                                 DWORD intervalMs);
BOOL multiPixelTrackerUpdate(MultiPixelTracker *tracker,
                             ULONGLONG *timestampMs);
const MultiPixelTrackedPoint *
multiPixelTrackerGetPoints(const MultiPixelTracker *tracker, size_t *count);
void multiPixelTrackerDestroy(MultiPixelTracker *tracker);