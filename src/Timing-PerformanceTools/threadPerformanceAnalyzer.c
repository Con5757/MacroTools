#include "threadPerformanceAnalyzer.h"
#include <string.h>

void ttThreadPerformanceAnalyzerInitialize(TTThreadPerformanceAnalyzer *a) {
  if (a)
    memset(a, 0, sizeof(*a));
}
BOOL ttThreadPerformanceAnalyzerUpdate(TTThreadPerformanceAnalyzer *a,
                                       HANDLE thread,
                                       TTThreadPerformanceResult *r) {
  FILETIME created, exitTime, kernel, user;
  LARGE_INTEGER now;
  uint64_t cpu;
  double wallMs;
  if (!a || !thread || !r ||
      !GetThreadTimes(thread, &created, &exitTime, &kernel, &user) ||
      !ttQueryPerformanceCounter(&now))
    return FALSE;
  memset(r, 0, sizeof(*r));
  if (!a->initialized) {
    a->kernel = kernel;
    a->user = user;
    a->wall = now;
    a->initialized = TRUE;
    r->threadId = GetThreadId(thread);
    return TRUE;
  }
  cpu = (ttFileTimeToUInt64(kernel) - ttFileTimeToUInt64(a->kernel)) +
        (ttFileTimeToUInt64(user) - ttFileTimeToUInt64(a->user));
  wallMs = ttCounterDifferenceMilliseconds(a->wall, now);
  r->threadCpuMilliseconds = (double)cpu / 10000.0;
  r->threadCpuPercent =
      wallMs > 0 ? 100.0 * r->threadCpuMilliseconds / wallMs : 0;
  r->threadId = GetThreadId(thread);
  r->valid = TRUE;
  a->kernel = kernel;
  a->user = user;
  a->wall = now;
  return TRUE;
}