#include "inputLatencyTester.h"
#include <string.h>

void ttInputLatencyTesterInitialize(TTInputLatencyTester *t) {
  if (t) {
    memset(t, 0, sizeof(*t));
    t->minimumMs = 1.0e300;
  }
}
BOOL ttInputLatencyTesterMark(TTInputLatencyTester *t) {
  if (!t || t->waiting || !ttQueryPerformanceCounter(&t->marker))
    return FALSE;
  t->waiting = TRUE;
  return TRUE;
}
BOOL ttInputLatencyTesterComplete(TTInputLatencyTester *t,
                                  TTInputLatencyResult *r) {
  LARGE_INTEGER now;
  double ms;
  if (!t || !r || !t->waiting || !ttQueryPerformanceCounter(&now))
    return FALSE;
  ms = ttCounterDifferenceMilliseconds(t->marker, now);
  t->waiting = FALSE;
  t->sampleCount++;
  t->totalMs += ms;
  if (ms < t->minimumMs)
    t->minimumMs = ms;
  if (ms > t->maximumMs)
    t->maximumMs = ms;
  r->lastLatencyMs = ms;
  r->averageLatencyMs = t->totalMs / (double)t->sampleCount;
  r->minimumLatencyMs = t->minimumMs;
  r->maximumLatencyMs = t->maximumMs;
  r->sampleCount = t->sampleCount;
  return TRUE;
}