#include "systemLoadMonitor.h"
#include <string.h>

void ttSystemLoadMonitorInitialize(TTSystemLoadMonitor *m) {
  if (m)
    memset(m, 0, sizeof(*m));
}
BOOL ttSystemLoadMonitorUpdate(TTSystemLoadMonitor *m, TTSystemLoadResult *r) {
  FILETIME idle, kernel, user;
  MEMORYSTATUSEX mem;
  if (!m || !r || !GetSystemTimes(&idle, &kernel, &user))
    return FALSE;
  memset(r, 0, sizeof(*r));
  memset(&mem, 0, sizeof(mem));
  mem.dwLength = sizeof(mem);
  if (!GlobalMemoryStatusEx(&mem))
    return FALSE;
  if (m->initialized) {
    uint64_t di = ttFileTimeToUInt64(idle) - ttFileTimeToUInt64(m->idle);
    uint64_t dk = ttFileTimeToUInt64(kernel) - ttFileTimeToUInt64(m->kernel);
    uint64_t du = ttFileTimeToUInt64(user) - ttFileTimeToUInt64(m->user);
    uint64_t total = dk + du;
    if (total) {
      r->cpuUsagePercent = 100.0 * (1.0 - (double)di / (double)total);
      if (r->cpuUsagePercent < 0.0)
        r->cpuUsagePercent = 0.0;
      if (r->cpuUsagePercent > 100.0)
        r->cpuUsagePercent = 100.0;
      r->cpuUsageValid = TRUE;
    }
  }
  m->idle = idle;
  m->kernel = kernel;
  m->user = user;
  m->initialized = TRUE;
  r->memoryLoadPercent = mem.dwMemoryLoad;
  r->totalPhysicalBytes = mem.ullTotalPhys;
  r->availablePhysicalBytes = mem.ullAvailPhys;
  r->totalPageFileBytes = mem.ullTotalPageFile;
  r->availablePageFileBytes = mem.ullAvailPageFile;
  r->processorCount = ttGetProcessorCount();
  r->valid = TRUE;
  return TRUE;
}