#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  GTImage previousFrame;
  uint8_t *previousPixels;
  uint64_t activityStartMs;
  uint64_t lastUpdateMs;
  bool initialized;
  bool activityActive;
} GTLoadingScreenDetector;

bool loadingScreenDetectorInitialize(GTLoadingScreenDetector *detector);
void loadingScreenDetectorDestroy(GTLoadingScreenDetector *detector);
bool loadingScreenDetectorUpdate(GTLoadingScreenDetector *detector,
                                 const GTImage *frame, uint64_t timestampMs,
                                 double activityThreshold,
                                 uint32_t minimumActivityDurationMs,
                                 bool *loadingDetected,
                                 double *frameDifference);