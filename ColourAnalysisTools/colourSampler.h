#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  PixelColor average;
  PixelColor minimum;
  PixelColor maximum;
  PixelColor center;
  size_t pixelCount;
} PixelColourSample;

BOOL colourSamplerSampleRegion(int x, int y, int width, int height,
                               PixelColourSample *sample);