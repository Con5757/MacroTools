#include "pollingRateAnalyzer.h"
#include <string.h>

void ttPollingRateAnalyzerInitialize(TTPollingRateAnalyzer *a) {
  if (a) {
    memset(a, 0, sizeof(*a));
    a->minimumIntervalMs = 1.0e300;
  }
}
BOOL ttPollingRateAnalyzerRecord(TTPollingRateAnalyzer *a,
                                 TTPollingRateResult *r) {
  LARGE_INTEGER now;
  double ms;
  if (!a || !r || !ttQueryPerformanceCounter(&now))
    return FALSE;
  memset(r, 0, sizeof(*r));
  if (!a->initialized) {
    a->lastEvent = now;
    a->initialized = TRUE;
    return TRUE;
  }
  ms = ttCounterDifferenceMilliseconds(a->lastEvent, now);
  a->lastEvent = now;
  a->eventCount++;
  a->totalIntervalMs += ms;
  if (ms < a->minimumIntervalMs)
    a->minimumIntervalMs = ms;
  if (ms > a->maximumIntervalMs)
    a->maximumIntervalMs = ms;
  r->eventCount = a->eventCount;
  r->lastIntervalMs = ms;
  r->averageIntervalMs = a->totalIntervalMs / (double)a->eventCount;
  r->pollingRateHz = ms > 0 ? 1000.0 / ms : 0;
  r->minimumIntervalMs = a->minimumIntervalMs;
  r->maximumIntervalMs = a->maximumIntervalMs;
  return TRUE;
}