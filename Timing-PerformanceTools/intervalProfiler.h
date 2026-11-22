#pragma once
#include "timingToolsCommon.h"

typedef struct {
  double requestedIntervalMs;
  LARGE_INTEGER lastSample;
  BOOL initialized;
  uint64_t sampleCount;
  double totalActualMs;
  double minimumActualMs;
  double maximumActualMs;
  double totalAbsoluteErrorMs;
  double cumulativeSignedErrorMs;
} TTIntervalProfiler;

typedef struct {
  uint64_t sampleCount;
  double requestedIntervalMs;
  double actualIntervalMs;
  double errorMs;
  double averageActualMs;
  double averageAbsoluteErrorMs;
  double cumulativeSignedErrorMs;
  double minimumActualMs;
  double maximumActualMs;
} TTIntervalProfilerResult;

BOOL ttIntervalProfilerInitialize(TTIntervalProfiler *profiler,
                                  double requestedIntervalMs);
BOOL ttIntervalProfilerRecord(TTIntervalProfiler *profiler,
                              TTIntervalProfilerResult *result);