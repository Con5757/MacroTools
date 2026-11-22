#include "memoryUsageMonitor.h"
#include <string.h>

BOOL ttMemoryUsageMonitorGetCurrentProcess(TTMemoryUsageResult *r) {
  PROCESS_MEMORY_COUNTERS_EX counters;
  if (!r)
    return FALSE;
  memset(&counters, 0, sizeof(counters));
  counters.cb = sizeof(counters);
  if (!GetProcessMemoryInfo(GetCurrentProcess(),
                            (PROCESS_MEMORY_COUNTERS *)&counters,
                            sizeof(counters)))
    return FALSE;
  r->workingSetBytes = counters.WorkingSetSize;
  r->peakWorkingSetBytes = counters.PeakWorkingSetSize;
  r->privateBytes = counters.PrivateUsage;
  r->pagefileBytes = counters.PagefileUsage;
  return TRUE;
}