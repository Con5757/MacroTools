#include "timingDriftAnalyzer.h"
#include <string.h>
void ttTimingDriftAnalyzerInitialize(TTTimingDriftAnalyzer *a) {
  if (a)
    memset(a, 0, sizeof(*a));
}
BOOL ttTimingDriftAnalyzerRecord(TTTimingDriftAnalyzer *a,
                                 LARGE_INTEGER scheduled,
                                 TTTimingDriftResult *r) {
  LARGE_INTEGER now;
  double drift, absolute;
  if (!a || !r || !ttQueryPerformanceCounter(&now))
    return FALSE;
  drift = ttCounterDifferenceMilliseconds(scheduled, now);
  absolute = drift < 0 ? -drift : drift;
  a->sampleCount++;
  a->lastDriftMs = drift;
  a->totalDriftMs += drift;
  a->totalAbsoluteDriftMs += absolute;
  if (absolute > a->maximumAbsoluteDriftMs)
    a->maximumAbsoluteDriftMs = absolute;
  r->sampleCount = a->sampleCount;
  r->lastDriftMs = drift;
  r->averageDriftMs = a->totalDriftMs / (double)a->sampleCount;
  r->averageAbsoluteDriftMs = a->totalAbsoluteDriftMs / (double)a->sampleCount;
  r->maximumAbsoluteDriftMs = a->maximumAbsoluteDriftMs;
  return TRUE;
}