#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  const char *previousState;
  const char *currentState;
  uint64_t timestampMs;
  bool changed;
} GTStateTransition;
typedef struct {
  const char *stableState;
  const char *candidateState;
  unsigned candidateFrames;
  unsigned requiredFrames;
} GTGameStateTransitionMonitor;

bool gameStateTransitionMonitorInitialize(GTGameStateTransitionMonitor *monitor,
                                          unsigned requiredStableFrames);
bool gameStateTransitionMonitorUpdate(GTGameStateTransitionMonitor *monitor,
                                      const char *identifiedState,
                                      uint64_t timestampMs,
                                      GTStateTransition *transition);