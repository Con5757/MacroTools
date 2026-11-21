#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  PixelColor color;
  size_t count;
  double percentage;
} PixelColourFrequency;

BOOL colourFrequencyAnalyzerAnalyze(int x, int y, int width, int height,
                                    unsigned int bitsToDiscardPerChannel,
                                    size_t maxResults,
                                    PixelColourFrequency **frequencies,
                                    size_t *frequencyCount);