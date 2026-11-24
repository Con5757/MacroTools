#pragma once
#include "timingToolsCommon.h"

typedef struct {
  LARGE_INTEGER lastEvent;
  BOOL initialized;
  uint64_t eventCount;
  double totalIntervalMs;
  double minimumIntervalMs;
  double maximumIntervalMs;
} TTPollingRateAnalyzer;
typedef struct {
  uint64_t eventCount;
  double lastIntervalMs;
  double averageIntervalMs;
  double pollingRateHz;
  double minimumIntervalMs;
  double maximumIntervalMs;
} TTPollingRateResult;

void ttPollingRateAnalyzerInitialize(TTPollingRateAnalyzer *analyzer);
BOOL ttPollingRateAnalyzerRecord(TTPollingRateAnalyzer *analyzer,
                                 TTPollingRateResult *result);