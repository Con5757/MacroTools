#pragma once
#include "timingToolsCommon.h"

typedef struct {
  LARGE_INTEGER start;
  uint64_t callCount;
  double totalMs;
  double minimumMs;
  double maximumMs;
  BOOL running;
} TTExecutionTimer;

typedef struct {
  double lastMs;
  double averageMs;
  double minimumMs;
  double maximumMs;
  uint64_t callCount;
} TTExecutionTimerResult;

void ttExecutionTimerInitialize(TTExecutionTimer *timer);
BOOL ttExecutionTimerBegin(TTExecutionTimer *timer);
BOOL ttExecutionTimerEnd(TTExecutionTimer *timer,
                         TTExecutionTimerResult *result);