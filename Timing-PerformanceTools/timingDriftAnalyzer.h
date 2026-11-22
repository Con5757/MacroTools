#pragma once
#include "timingToolsCommon.h"

typedef struct {
  uint64_t sampleCount;
  double totalDriftMs;
  double totalAbsoluteDriftMs;
  double maximumAbsoluteDriftMs;
  double lastDriftMs;
} TTTimingDriftAnalyzer;
typedef struct {
  uint64_t sampleCount;
  double lastDriftMs;
  double averageDriftMs;
  double averageAbsoluteDriftMs;
  double maximumAbsoluteDriftMs;
} TTTimingDriftResult;

void ttTimingDriftAnalyzerInitialize(TTTimingDriftAnalyzer *analyzer);
BOOL ttTimingDriftAnalyzerRecord(TTTimingDriftAnalyzer *analyzer,
                                 LARGE_INTEGER scheduledTime,
                                 TTTimingDriftResult *result);