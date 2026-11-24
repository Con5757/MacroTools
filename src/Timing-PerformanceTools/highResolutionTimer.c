#include "highResolutionTimer.h"

BOOL ttHighResolutionTimerStart(TTHighResolutionTimer *t) {
    if (!t || !ttQueryPerformanceCounter(&t->start)) return FALSE;
    t->end.QuadPart = 0; t->running = TRUE; t->completed = FALSE; return TRUE;
}
BOOL ttHighResolutionTimerStop(TTHighResolutionTimer *t, double *ms) {
    if (!t || !ms || !t->running || !ttQueryPerformanceCounter(&t->end)) return FALSE;
    t->running = FALSE; t->completed = TRUE;
    *ms = ttCounterDifferenceMilliseconds(t->start, t->end); return TRUE;
}
BOOL ttHighResolutionTimerGetElapsed(const TTHighResolutionTimer *t, double *ms) {
    LARGE_INTEGER now;
    if (!t || !ms || (!t->running && !t->completed)) return FALSE;
    if (t->running) { if (!ttQueryPerformanceCounter(&now)) return FALSE; }
    else now = t->end;
    *ms = ttCounterDifferenceMilliseconds(t->start, now); return TRUE;
}