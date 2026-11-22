#include "cpuUsageMonitor.h"
#include <string.h>

void ttCpuUsageMonitorInitialize(TTCpuUsageMonitor *m) {
  if (m)
    memset(m, 0, sizeof(*m));
}
BOOL ttCpuUsageMonitorUpdate(TTCpuUsageMonitor *m, TTCpuUsageResult *r) {
  FILETIME idle, kernel, user, created, exitTime, pk, pu;
  LARGE_INTEGER wall;
  uint64_t di, dk, du, dp;
  double total, process, elapsed;
  if (!m || !r || !GetSystemTimes(&idle, &kernel, &user) ||
      !GetProcessTimes(GetCurrentProcess(), &created, &exitTime, &pk, &pu) ||
      !ttQueryPerformanceCounter(&wall))
    return FALSE;
  memset(r, 0, sizeof(*r));
  if (!m->initialized) {
    m->idle = idle;
    m->kernel = kernel;
    m->user = user;
    m->processKernel = pk;
    m->processUser = pu;
    m->wall = wall;
    m->initialized = TRUE;
    return TRUE;
  }
  di = ttFileTimeToUInt64(idle) - ttFileTimeToUInt64(m->idle);
  dk = ttFileTimeToUInt64(kernel) - ttFileTimeToUInt64(m->kernel);
  du = ttFileTimeToUInt64(user) - ttFileTimeToUInt64(m->user);
  dp = (ttFileTimeToUInt64(pk) - ttFileTimeToUInt64(m->processKernel)) +
       (ttFileTimeToUInt64(pu) - ttFileTimeToUInt64(m->processUser));
  total = (double)(dk + du);
  elapsed = ttCounterDifferenceMilliseconds(m->wall, wall);
  process = (double)dp / 10000.0;
  if (total > 0)
    r->systemCpuPercent = 100.0 * (1.0 - (double)di / total);
  if (elapsed > 0)
    r->processCpuPercentPerCore = 100.0 * process / elapsed;
  r->processCpuPercent =
      r->processCpuPercentPerCore / (double)ttGetProcessorCount();
  if (r->systemCpuPercent < 0)
    r->systemCpuPercent = 0;
  if (r->systemCpuPercent > 100)
    r->systemCpuPercent = 100;
  if (r->processCpuPercent < 0)
    r->processCpuPercent = 0;
  m->idle = idle;
  m->kernel = kernel;
  m->user = user;
  m->processKernel = pk;
  m->processUser = pu;
  m->wall = wall;
  r->valid = TRUE;
  return TRUE;
}