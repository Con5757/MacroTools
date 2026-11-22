#include "intervalProfiler.h"
#include <string.h>

BOOL ttIntervalProfilerInitialize(TTIntervalProfiler *p, double intervalMs) {
  if (!p || intervalMs <= 0.0)
    return FALSE;
  memset(p, 0, sizeof(*p));
  p->requestedIntervalMs = intervalMs;
  p->minimumActualMs = 1.0e300;
  return TRUE;
}
BOOL ttIntervalProfilerRecord(TTIntervalProfiler *p,
                              TTIntervalProfilerResult *r) {
  LARGE_INTEGER now;
  if (!p || !r || !ttQueryPerformanceCounter(&now))
    return FALSE;
  memset(r, 0, sizeof(*r));
  if (!p->initialized) {
    p->lastSample = now;
    p->initialized = TRUE;
    r->requestedIntervalMs = p->requestedIntervalMs;
    r->minimumActualMs = 0.0;
    return TRUE;
  }
  double actual = ttCounterDifferenceMilliseconds(p->lastSample, now);
  double error = actual - p->requestedIntervalMs;
  p->lastSample = now;
  p->sampleCount++;
  p->totalActualMs += actual;
  p->totalAbsoluteErrorMs += error < 0 ? -error : error;
  p->cumulativeSignedErrorMs += error;
  if (actual < p->minimumActualMs)
    p->minimumActualMs = actual;
  if (actual > p->maximumActualMs)
    p->maximumActualMs = actual;
  r->sampleCount = p->sampleCount;
  r->requestedIntervalMs = p->requestedIntervalMs;
  r->actualIntervalMs = actual;
  r->errorMs = error;
  r->averageActualMs = p->totalActualMs / (double)p->sampleCount;
  r->averageAbsoluteErrorMs = p->totalAbsoluteErrorMs / (double)p->sampleCount;
  r->cumulativeSignedErrorMs = p->cumulativeSignedErrorMs;
  r->minimumActualMs = p->minimumActualMs;
  r->maximumActualMs = p->maximumActualMs;
  return TRUE;
}