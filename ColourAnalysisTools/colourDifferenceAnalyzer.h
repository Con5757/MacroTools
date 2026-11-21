#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  int redDifference;
  int greenDifference;
  int blueDifference;
  double rgbEuclideanDistance;
  double rgbManhattanDistance;
  double normalizedDifferencePercent;
  double deltaE76;
} PixelColourDifference;

void colourDifferenceAnalyzerCompare(PixelColor first, PixelColor second,
                                     PixelColourDifference *difference);