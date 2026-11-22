#include "frameRateMonitor.h"
#include <string.h>

void ttFrameRateMonitorInitialize(TTFrameRateMonitor *m) {
  if (m) {
    memset(m, 0, sizeof(*m));
    m->minimumFrameMs = 1.0e300;
  }
}
BOOL ttFrameRateMonitorUpdate(TTFrameRateMonitor *m, TTFrameRateResult *r) {
  LARGE_INTEGER now;
  double ms;
  if (!m || !r || !ttQueryPerformanceCounter(&now))
    return FALSE;
  memset(r, 0, sizeof(*r));
  if (!m->initialized) {
    m->lastFrame = now;
    m->initialized = TRUE;
    return TRUE;
  }
  ms = ttCounterDifferenceMilliseconds(m->lastFrame, now);
  m->lastFrame = now;
  m->frameCount++;
  m->totalElapsedMs += ms;
  if (ms < m->minimumFrameMs)
    m->minimumFrameMs = ms;
  if (ms > m->maximumFrameMs)
    m->maximumFrameMs = ms;
  r->frameCount = m->frameCount;
  r->frameTimeMs = ms;
  r->instantaneousFps = ms > 0 ? 1000.0 / ms : 0;
  r->averageFps = m->totalElapsedMs > 0
                      ? (double)m->frameCount * 1000.0 / m->totalElapsedMs
                      : 0;
  r->minimumFrameMs = m->minimumFrameMs;
  r->maximumFrameMs = m->maximumFrameMs;
  return TRUE;
}