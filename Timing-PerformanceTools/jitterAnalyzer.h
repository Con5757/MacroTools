#pragma once
#include "timingToolsCommon.h"

typedef struct {
  LARGE_INTEGER lastEvent;
  BOOL initialized;
  uint64_t intervalCount;
  double meanIntervalMs;
  double m2IntervalMs;
  double minimumIntervalMs;
  double maximumIntervalMs;
} TTJitterAnalyzer;
typedef struct {
  uint64_t intervalCount;
  double lastIntervalMs;
  double meanIntervalMs;
  double jitterStdDevMs;
  double minimumIntervalMs;
  double maximumIntervalMs;
} TTJitterResult;

void ttJitterAnalyzerInitialize(TTJitterAnalyzer *analyzer);
BOOL ttJitterAnalyzerRecord(TTJitterAnalyzer *analyzer, TTJitterResult *result);