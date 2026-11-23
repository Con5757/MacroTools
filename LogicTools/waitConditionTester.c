#include "waitConditionTester.h"

bool laWaitBegin(LAWaitCondition *w, LAConditionCallback fn, void *ctx,
                 uint64_t now, uint64_t timeout, uint64_t interval) {
  if (!w || !fn)
    return false;
  w->condition = fn;
  w->context = ctx;
  w->startMs = now;
  w->timeoutMs = timeout;
  w->intervalMs = interval;
  w->nextCheckMs = now;
  w->status = LA_WAIT_PENDING;
  return true;
}
LAWaitStatus laWaitUpdate(LAWaitCondition *w, uint64_t now) {
  if (!w || w->status == LA_WAIT_INVALID)
    return LA_WAIT_INVALID;
  if (w->status != LA_WAIT_PENDING)
    return w->status;
  if (now < w->nextCheckMs)
    return w->status;
  if (w->condition(w->context)) {
    w->status = LA_WAIT_SATISFIED;
    return w->status;
  }
  if (now >= w->startMs && now - w->startMs >= w->timeoutMs) {
    w->status = LA_WAIT_TIMED_OUT;
    return w->status;
  }
  w->nextCheckMs = laAddMilliseconds(now, w->intervalMs);
  return w->status;
}
void laWaitCancel(LAWaitCondition *w) {
  if (w)
    w->status = LA_WAIT_INVALID;
}