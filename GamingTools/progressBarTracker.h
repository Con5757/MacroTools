#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>

typedef struct {
  double progress;
  size_t filledSamples;
  size_t totalSamples;
  bool valid;
} GTProgressResult;

bool progressBarTrackerUpdate(const GTImage *frame, GTRect region,
                              GTColor fillColor, double tolerance,
                              unsigned sampleStep, GTProgressResult *result);