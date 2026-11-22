#pragma once
#include "timingToolsCommon.h"

typedef struct {
  LARGE_INTEGER marker;
  uint64_t sampleCount;
  double totalMs;
  double minimumMs;
  double maximumMs;
  BOOL waiting;
} TTInputLatencyTester;
typedef struct {
  double lastLatencyMs;
  double averageLatencyMs;
  double minimumLatencyMs;
  double maximumLatencyMs;
  uint64_t sampleCount;
} TTInputLatencyResult;

void ttInputLatencyTesterInitialize(TTInputLatencyTester *tester);
BOOL ttInputLatencyTesterMark(TTInputLatencyTester *tester);
BOOL ttInputLatencyTesterComplete(TTInputLatencyTester *tester,
                                  TTInputLatencyResult *result);