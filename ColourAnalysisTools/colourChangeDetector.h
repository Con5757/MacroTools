#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  int x;
  int y;
  DWORD intervalMs;
  double threshold;
  ULONGLONG lastSampleTime;
  PixelColor previousColor;
  BOOL initialized;
} PixelColourChangeDetector;

BOOL colourChangeDetectorInitialize(PixelColourChangeDetector *detector, int x,
                                    int y, DWORD intervalMs, double threshold);
BOOL colourChangeDetectorUpdate(PixelColourChangeDetector *detector,
                                BOOL *changed, PixelColor *previous,
                                PixelColor *current, double *difference,
                                ULONGLONG *timestampMs);