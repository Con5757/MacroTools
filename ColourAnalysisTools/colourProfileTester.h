#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  char monitorName[CCHDEVICENAME];
  char iccProfilePath[MAX_PATH];
  BOOL iccProfileAvailable;
  int bitsPerPixel;
  RECT monitorBounds;
  PixelColor sampledColor;
  PixelColor expectedColor;
  double rgbDistance;
} PixelColourProfileReport;

typedef struct {
  double sampledColourDistance;
  double firstDetectionError;
  double secondDetectionError;
  double detectionErrorDifference;
  BOOL sameIccProfile;
  BOOL sameBitDepth;
} PixelColourProfileComparison;

BOOL colourProfileTesterInspect(int x, int y, PixelColor expectedColor,
                                PixelColourProfileReport *report);
BOOL colourProfileTesterCompare(const PixelColourProfileReport *first,
                                const PixelColourProfileReport *second,
                                PixelColourProfileComparison *comparison);