#pragma once
#include "timingToolsCommon.h"

typedef struct {
  DWORD requestedSleepMs;
  double actualSleepMs;
  double overshootMs;
} TTSleepAccuracyResult;

BOOL ttSleepAccuracyTesterRun(DWORD requestedSleepMs,
                              TTSleepAccuracyResult *result);