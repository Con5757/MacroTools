#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  int x;
  int y;
  DWORD intervalMs;
  ULONGLONG lastSampleTime;
  BOOL hasSample;
  PixelColor lastColor;
} PixelTracker;

BOOL pixelTrackerInitialize(PixelTracker *tracker, int x, int y,
                            DWORD intervalMs);
BOOL pixelTrackerUpdate(PixelTracker *tracker, PixelColor *color,
                        ULONGLONG *timestampMs);
