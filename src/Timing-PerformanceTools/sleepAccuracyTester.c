#include "sleepAccuracyTester.h"

BOOL ttSleepAccuracyTesterRun(DWORD requested, TTSleepAccuracyResult *r) {
  LARGE_INTEGER start, end;
  if (!r || !ttQueryPerformanceCounter(&start))
    return FALSE;
  Sleep(requested);
  if (!ttQueryPerformanceCounter(&end))
    return FALSE;
  r->requestedSleepMs = requested;
  r->actualSleepMs = ttCounterDifferenceMilliseconds(start, end);
  r->overshootMs = r->actualSleepMs - (double)requested;
  return TRUE;
}