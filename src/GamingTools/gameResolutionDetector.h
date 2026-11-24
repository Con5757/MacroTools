#pragma once

#include "gameWindowDetector.h"
#include <stdbool.h>

typedef struct {
  int width;
  int height;
  double aspectRatio;
  int desktopX;
  int desktopY;
} GTGameResolution;

bool gameResolutionDetectorRead(GTWindowHandle window,
                                GTGameResolution *resolution);