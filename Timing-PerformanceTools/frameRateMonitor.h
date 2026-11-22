#pragma once
#include "timingToolsCommon.h"

typedef struct {
  LARGE_INTEGER lastFrame;
  BOOL initialized;
  uint64_t frameCount;
  double totalElapsedMs;
  double minimumFrameMs;
  double maximumFrameMs;
} TTFrameRateMonitor;

typedef struct {
  uint64_t frameCount;
  double frameTimeMs;
  double instantaneousFps;
  double averageFps;
  double minimumFrameMs;
  double maximumFrameMs;
} TTFrameRateResult;

void ttFrameRateMonitorInitialize(TTFrameRateMonitor *monitor);
BOOL ttFrameRateMonitorUpdate(TTFrameRateMonitor *monitor,
                              TTFrameRateResult *result);