#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  PixelColor minimum;
  PixelColor maximum;
} PixelRGBRange;

typedef struct {
  double minimumHue;
  double maximumHue;
  double minimumSaturation;
  double maximumSaturation;
  double minimumValue;
  double maximumValue;
} PixelHSVRange;

BOOL colourRangeFinderSearchRGB(int x, int y, int width, int height,
                                PixelRGBRange range, PixelPointArray *matches);
BOOL colourRangeFinderSearchHSV(int x, int y, int width, int height,
                                PixelHSVRange range, PixelPointArray *matches);