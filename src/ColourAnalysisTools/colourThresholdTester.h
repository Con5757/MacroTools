#pragma once

#include "pixelToolsCommon.h"

typedef enum {
  PIXEL_COLOUR_THRESHOLD_PER_CHANNEL = 0,
  PIXEL_COLOUR_THRESHOLD_EUCLIDEAN = 1
} PixelColourThresholdMode;

typedef struct {
  PixelColourThresholdMode mode;
  int perChannelTolerance;
  double euclideanTolerance;
} PixelColourThreshold;

typedef struct {
  int redDifference;
  int greenDifference;
  int blueDifference;
  double euclideanDistance;
  BOOL matched;
} PixelColourThresholdResult;

BOOL colourThresholdTesterTest(PixelColor expected, PixelColor actual,
                               PixelColourThreshold threshold,
                               PixelColourThresholdResult *result);