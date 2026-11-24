#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t *previousPixels;
  int previousWidth;
  int previousHeight;
  size_t previousStride;
  uint64_t lastTimestampMs;
  bool initialized;
} GTGameNotificationDetector;

bool gameNotificationDetectorInitialize(GTGameNotificationDetector *detector);
void gameNotificationDetectorDestroy(GTGameNotificationDetector *detector);
bool gameNotificationDetectorUpdate(GTGameNotificationDetector *detector,
                                    const GTImage *frame, GTRect region,
                                    uint64_t timestampMs,
                                    double meanDifferenceThreshold,
                                    bool *detected, double *difference);