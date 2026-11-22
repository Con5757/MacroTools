#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputEventTimestampStats {
  size_t eventCount;
  size_t intervalCount;
  size_t outOfOrderCount;
  ULONGLONG minimumIntervalMs;
  ULONGLONG maximumIntervalMs;
  ULONGLONG durationMs;
  double meanIntervalMs;
  double standardDeviationMs;
} InputEventTimestampStats;

BOOL inputEventTimestampAnalyzerAnalyze(const InputEvent *events, size_t count,
                                        InputEventTimestampStats *stats);

#ifdef __cplusplus
}
#endif