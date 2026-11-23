#pragma once
#include "logicToolsCommon.h"

typedef enum {
  LA_WAIT_INVALID,
  LA_WAIT_PENDING,
  LA_WAIT_SATISFIED,
  LA_WAIT_TIMED_OUT
} LAWaitStatus;
typedef struct {
  LAConditionCallback condition;
  void *context;
  uint64_t startMs, timeoutMs, intervalMs, nextCheckMs;
  LAWaitStatus status;
} LAWaitCondition;

bool laWaitBegin(LAWaitCondition *wait, LAConditionCallback condition,
                 void *context, uint64_t nowMs, uint64_t timeoutMs,
                 uint64_t pollIntervalMs);
LAWaitStatus laWaitUpdate(LAWaitCondition *wait, uint64_t nowMs);
void laWaitCancel(LAWaitCondition *wait);