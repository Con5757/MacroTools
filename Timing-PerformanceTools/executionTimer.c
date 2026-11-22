#include "executionTimer.h"
#include <string.h>

void ttExecutionTimerInitialize(TTExecutionTimer *t) {
  if (t) {
    memset(t, 0, sizeof(*t));
    t->minimumMs = 1.0e300;
  }
}
BOOL ttExecutionTimerBegin(TTExecutionTimer *t) {
  if (!t || t->running || !ttQueryPerformanceCounter(&t->start))
    return FALSE;
  t->running = TRUE;
  return TRUE;
}
BOOL ttExecutionTimerEnd(TTExecutionTimer *t, TTExecutionTimerResult *r) {
  LARGE_INTEGER end;
  double ms;
  if (!t || !r || !t->running || !ttQueryPerformanceCounter(&end))
    return FALSE;
  ms = ttCounterDifferenceMilliseconds(t->start, end);
  t->running = FALSE;
  t->callCount++;
  t->totalMs += ms;
  if (ms < t->minimumMs)
    t->minimumMs = ms;
  if (ms > t->maximumMs)
    t->maximumMs = ms;
  r->lastMs = ms;
  r->averageMs = t->totalMs / (double)t->callCount;
  r->minimumMs = t->minimumMs;
  r->maximumMs = t->maximumMs;
  r->callCount = t->callCount;
  return TRUE;
}