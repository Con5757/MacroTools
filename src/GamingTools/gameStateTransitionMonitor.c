#include "gameStateTransitionMonitor.h"
#include <string.h>

bool gameStateTransitionMonitorInitialize(GTGameStateTransitionMonitor *monitor,
                                          unsigned requiredStableFrames) {
  if (monitor == NULL || requiredStableFrames == 0)
    return false;
  monitor->stableState = NULL;
  monitor->candidateState = NULL;
  monitor->candidateFrames = 0;
  monitor->requiredFrames = requiredStableFrames;
  return true;
}
bool gameStateTransitionMonitorUpdate(GTGameStateTransitionMonitor *monitor,
                                      const char *identifiedState,
                                      uint64_t timestampMs,
                                      GTStateTransition *transition) {
  if (monitor == NULL || transition == NULL || identifiedState == NULL ||
      monitor->requiredFrames == 0)
    return false;
  memset(transition, 0, sizeof(*transition));
  transition->timestampMs = timestampMs;

  if (monitor->stableState != NULL &&
      strcmp(monitor->stableState, identifiedState) == 0) {
    monitor->candidateState = NULL;
    monitor->candidateFrames = 0;
    return true;
  }
  if (monitor->candidateState == NULL ||
      strcmp(monitor->candidateState, identifiedState) != 0) {
    monitor->candidateState = identifiedState;
    monitor->candidateFrames = 1;
  } else if (monitor->candidateFrames < monitor->requiredFrames) {
    monitor->candidateFrames++;
  }

  if (monitor->candidateFrames >= monitor->requiredFrames) {
    transition->previousState = monitor->stableState;
    transition->currentState = monitor->candidateState;
    transition->changed = true;
    monitor->stableState = monitor->candidateState;
    monitor->candidateState = NULL;
    monitor->candidateFrames = 0;
  }
  return true;
}