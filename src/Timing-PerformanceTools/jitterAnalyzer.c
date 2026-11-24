#include "jitterAnalyzer.h"
#include <math.h>
#include <string.h>

void ttJitterAnalyzerInitialize(TTJitterAnalyzer *a) {
  if (a) {
    memset(a, 0, sizeof(*a));
    a->minimumIntervalMs = 1.0e300;
  }
}
BOOL ttJitterAnalyzerRecord(TTJitterAnalyzer *a, TTJitterResult *r) {
  LARGE_INTEGER now;
  double x, delta;
  if (!a || !r || !ttQueryPerformanceCounter(&now))
    return FALSE;
  memset(r, 0, sizeof(*r));
  if (!a->initialized) {
    a->lastEvent = now;
    a->initialized = TRUE;
    return TRUE;
  }
  x = ttCounterDifferenceMilliseconds(a->lastEvent, now);
  a->lastEvent = now;
  a->intervalCount++;
  delta = x - a->meanIntervalMs;
  a->meanIntervalMs += delta / (double)a->intervalCount;
  a->m2IntervalMs += delta * (x - a->meanIntervalMs);
  if (x < a->minimumIntervalMs)
    a->minimumIntervalMs = x;
  if (x > a->maximumIntervalMs)
    a->maximumIntervalMs = x;
  r->intervalCount = a->intervalCount;
  r->lastIntervalMs = x;
  r->meanIntervalMs = a->meanIntervalMs;
  r->jitterStdDevMs =
      a->intervalCount > 1
          ? sqrt(a->m2IntervalMs / (double)(a->intervalCount - 1))
          : 0;
  r->minimumIntervalMs = a->minimumIntervalMs;
  r->maximumIntervalMs = a->maximumIntervalMs;
  return TRUE;
}