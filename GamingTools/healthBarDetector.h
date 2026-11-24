#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>

typedef enum { GT_BAR_HORIZONTAL = 0, GT_BAR_VERTICAL = 1 } GTBarOrientation;
typedef struct {
  double fillRatio;
  size_t matchedPixels;
  size_t totalPixels;
  bool valid;
} GTHealthBarResult;

bool healthBarDetectorMeasure(const GTImage *frame, GTRect region,
                              GTColor fillColor, double colorTolerance,
                              GTBarOrientation orientation, unsigned sampleStep,
                              GTHealthBarResult *result);