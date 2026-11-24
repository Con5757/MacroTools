#pragma once
#include "timingToolsCommon.h"

typedef struct {
  LARGE_INTEGER start;
  LARGE_INTEGER end;
  BOOL running;
  BOOL completed;
} TTHighResolutionTimer;

BOOL ttHighResolutionTimerStart(TTHighResolutionTimer *timer);
BOOL ttHighResolutionTimerStop(TTHighResolutionTimer *timer, double *elapsedMs);
BOOL ttHighResolutionTimerGetElapsed(const TTHighResolutionTimer *timer,
                                     double *elapsedMs);