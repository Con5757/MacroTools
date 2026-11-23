#include "taskScheduler.h"
#include <string.h>

void laTaskSchedulerInitialize(LATaskScheduler *s) {
  if (s)
    memset(s, 0, sizeof(*s));
}
bool laTaskSchedule(LATaskScheduler *s, uint32_t id, uint64_t first,
                    uint64_t interval, bool repeat, LATaskCallback cb,
                    void *ctx) {
  if (!s || !cb || (repeat && interval == 0))
    return false;
  LATask *freeSlot = NULL;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++) {
    if (s->tasks[i].occupied && s->tasks[i].id == id) {
      freeSlot = &s->tasks[i];
      break;
    }
    if (!s->tasks[i].occupied && !freeSlot)
      freeSlot = &s->tasks[i];
  }
  if (!freeSlot)
    return false;
  *freeSlot = (LATask){id, first, interval, cb, ctx, repeat, true};
  return true;
}
bool laTaskCancel(LATaskScheduler *s, uint32_t id) {
  if (!s)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++)
    if (s->tasks[i].occupied && s->tasks[i].id == id) {
      memset(&s->tasks[i], 0, sizeof(s->tasks[i]));
      return true;
    }
  return false;
}
size_t laTaskSchedulerUpdate(LATaskScheduler *s, uint64_t now, size_t max) {
  if (!s)
    return 0;
  size_t ran = 0;
  for (size_t i = 0; i < LA_MAX_ITEMS && ran < max; i++) {
    LATask *t = &s->tasks[i];
    if (!t->occupied || now < t->nextRunMs)
      continue;
    LATask snapshot = *t;
    if (t->repeat) {
      uint64_t elapsed = now >= t->nextRunMs ? now - t->nextRunMs : 0;
      uint64_t skipped = elapsed / t->intervalMs + 1;
      uint64_t maxSteps = (UINT64_MAX - t->nextRunMs) / t->intervalMs;
      if (skipped > maxSteps) {
        t->nextRunMs = UINT64_MAX;
        t->occupied = false;
      } else {
        t->nextRunMs += skipped * t->intervalMs;
      }
    } else {
      t->occupied = false;
    }
    snapshot.callback(snapshot.context, snapshot.id, now);
    ran++;
  }
  return ran;
}