#include "performanceBenchmark.h"
#include <string.h>

void ttPerformanceBenchmarkInitialize(TTPerformanceBenchmark *b) {
  if (b) {
    memset(b, 0, sizeof(*b));
    b->minimumMs = 1.0e300;
  }
}
BOOL ttPerformanceBenchmarkBegin(TTPerformanceBenchmark *b) {
  if (!b || b->running || !ttQueryPerformanceCounter(&b->start))
    return FALSE;
  b->running = TRUE;
  return TRUE;
}
BOOL ttPerformanceBenchmarkEnd(TTPerformanceBenchmark *b,
                               TTPerformanceBenchmarkResult *r) {
  LARGE_INTEGER end;
  double ms;
  if (!b || !r || !b->running || !ttQueryPerformanceCounter(&end))
    return FALSE;
  ms = ttCounterDifferenceMilliseconds(b->start, end);
  b->running = FALSE;
  b->iterations++;
  b->totalMs += ms;
  if (ms < b->minimumMs)
    b->minimumMs = ms;
  if (ms > b->maximumMs)
    b->maximumMs = ms;
  r->lastIterationMs = ms;
  r->averageIterationMs = b->totalMs / (double)b->iterations;
  r->minimumIterationMs = b->minimumMs;
  r->maximumIterationMs = b->maximumMs;
  r->operationsPerSecond = ms > 0 ? 1000.0 / ms : 0;
  r->iterations = b->iterations;
  return TRUE;
}