#pragma once
#include "timingToolsCommon.h"

typedef struct {
  FILETIME idle;
  FILETIME kernel;
  FILETIME user;
  FILETIME processKernel;
  FILETIME processUser;
  LARGE_INTEGER wall;
  BOOL initialized;
} TTCpuUsageMonitor;

typedef struct {
  double systemCpuPercent;
  double processCpuPercent;
  double processCpuPercentPerCore;
  BOOL valid;
} TTCpuUsageResult;

void ttCpuUsageMonitorInitialize(TTCpuUsageMonitor *monitor);
BOOL ttCpuUsageMonitorUpdate(TTCpuUsageMonitor *monitor,
                             TTCpuUsageResult *result);