#pragma once
#include "timingToolsCommon.h"

typedef struct {
  FILETIME kernel;
  FILETIME user;
  LARGE_INTEGER wall;
  BOOL initialized;
} TTThreadPerformanceAnalyzer;
typedef struct {
  double threadCpuPercent;
  double threadCpuMilliseconds;
  DWORD threadId;
  BOOL valid;
} TTThreadPerformanceResult;

void ttThreadPerformanceAnalyzerInitialize(
    TTThreadPerformanceAnalyzer *analyzer);
BOOL ttThreadPerformanceAnalyzerUpdate(TTThreadPerformanceAnalyzer *analyzer,
                                       HANDLE threadHandle,
                                       TTThreadPerformanceResult *result);