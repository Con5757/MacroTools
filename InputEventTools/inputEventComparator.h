#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputEventComparison {
  size_t compared;
  size_t matching;
  size_t mismatched;
  size_t leftOnly;
  size_t rightOnly;
  size_t timingMismatches;
  size_t payloadMismatches;
  ULONGLONG maxTimestampDifferenceMs;
  double meanTimestampDifferenceMs;
  BOOL equivalent;
} InputEventComparison;

BOOL inputEventComparatorCompare(const InputEvent *left, size_t leftCount,
                                 const InputEvent *right, size_t rightCount,
                                 ULONGLONG timestampToleranceMs,
                                 LONG coordinateTolerance,
                                 InputEventComparison *result);

#ifdef __cplusplus
}
#endif