#pragma once
#include "timingToolsCommon.h"

typedef struct {
  LARGE_INTEGER start;
  uint64_t iterations;
  double totalMs;
  double minimumMs;
  double maximumMs;
  BOOL running;
} TTPerformanceBenchmark;
typedef struct {
  double lastIterationMs;
  double averageIterationMs;
  double minimumIterationMs;
  double maximumIterationMs;
  double operationsPerSecond;
  uint64_t iterations;
} TTPerformanceBenchmarkResult;

void ttPerformanceBenchmarkInitialize(TTPerformanceBenchmark *benchmark);
BOOL ttPerformanceBenchmarkBegin(TTPerformanceBenchmark *benchmark);
BOOL ttPerformanceBenchmarkEnd(TTPerformanceBenchmark *benchmark,
                               TTPerformanceBenchmarkResult *result);