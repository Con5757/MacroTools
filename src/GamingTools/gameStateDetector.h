#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  const char *name;
  GTImage reference;
  double maximumMeanDifference;
} GTGameStateReference;
typedef struct {
  size_t referenceIndex;
  const char *stateName;
  double difference;
} GTGameStateResult;

bool gameStateDetectorIdentify(const GTImage *frame,
                               const GTGameStateReference *references,
                               size_t referenceCount,
                               GTGameStateResult *result);