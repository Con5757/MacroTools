#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>

typedef struct {
  double readiness;
  bool ready;
  size_t readyPixels;
  size_t sampledPixels;
} GTCooldownResult;

// estimates readiness in a region
bool cooldownTrackerMeasure(const GTImage *frame, GTRect region,
                            GTColor readyColor, double tolerance,
                            double readyThreshold, GTCooldownResult *result);