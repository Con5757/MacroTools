#pragma once
#include "timingToolsCommon.h"

typedef struct {
  FILETIME idle;
  FILETIME kernel;
  FILETIME user;
  BOOL initialized;
} TTSystemLoadMonitor;
typedef struct {
  double cpuUsagePercent;
  DWORD memoryLoadPercent;
  uint64_t totalPhysicalBytes;
  uint64_t availablePhysicalBytes;
  uint64_t totalPageFileBytes;
  uint64_t availablePageFileBytes;
  DWORD processorCount;
  BOOL cpuUsageValid;
  BOOL valid;
} TTSystemLoadResult;

void ttSystemLoadMonitorInitialize(TTSystemLoadMonitor *monitor);
BOOL ttSystemLoadMonitorUpdate(TTSystemLoadMonitor *monitor,
                               TTSystemLoadResult *result);