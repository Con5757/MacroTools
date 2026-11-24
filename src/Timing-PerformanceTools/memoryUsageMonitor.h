#pragma once
#include "timingToolsCommon.h"
#include <psapi.h>

typedef struct {
  SIZE_T workingSetBytes;
  SIZE_T peakWorkingSetBytes;
  SIZE_T privateBytes;
  SIZE_T pagefileBytes;
} TTMemoryUsageResult;

BOOL ttMemoryUsageMonitorGetCurrentProcess(TTMemoryUsageResult *result);