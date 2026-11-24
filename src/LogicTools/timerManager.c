#include "timerManager.h"
#include <string.h>

static LATimer *findTimer(LATimerManager *manager, uint32_t id, bool create) {
  LATimer *freeSlot = NULL;
  if (manager == NULL)
    return NULL;

  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    LATimer *timer = &manager->timers[i];
    if (timer->occupied && timer->id == id) {
      return timer;
    }
    if (!timer->active && freeSlot == NULL)
      freeSlot = timer;
  }

  if (create && freeSlot != NULL) {
    memset(freeSlot, 0, sizeof(*freeSlot));
    freeSlot->id = id;
    return freeSlot;
  }
  return NULL;
}
void laTimerManagerInitialize(LATimerManager *manager) {
  if (manager != NULL)
    memset(manager, 0, sizeof(*manager));
}
bool laTimerStart(LATimerManager *manager, uint32_t id, uint64_t nowMs,
                  uint64_t durationMs) {
  LATimer *timer = findTimer(manager, id, true);
  if (timer == NULL)
    return false;
  timer->id = id;
  timer->occupied = true;
  timer->startMs = nowMs;
  timer->durationMs = durationMs;
  timer->active = true;
  return true;
}
bool laTimerStop(LATimerManager *manager, uint32_t id) {
  if (manager == NULL)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    LATimer *timer = &manager->timers[i];
    if (timer->active && timer->id == id) {
      timer->active = false;
      return true;
    }
  }
  return false;
}
bool laTimerIsActive(const LATimerManager *manager, uint32_t id) {
  if (manager == NULL)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    if (manager->timers[i].active && manager->timers[i].id == id)
      return true;
  }
  return false;
}
bool laTimerElapsed(const LATimerManager *manager, uint32_t id, uint64_t nowMs,
                    uint64_t *elapsedMs) {
  if (manager == NULL || elapsedMs == NULL)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    const LATimer *timer = &manager->timers[i];
    if (timer->active && timer->id == id) {
      *elapsedMs = nowMs >= timer->startMs ? nowMs - timer->startMs : 0;
      return true;
    }
  }
  return false;
}
bool laTimerIsExpired(const LATimerManager *manager, uint32_t id,
                      uint64_t nowMs) {
  uint64_t elapsedMs;
  if (!laTimerElapsed(manager, id, nowMs, &elapsedMs))
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    const LATimer *timer = &manager->timers[i];
    if (timer->active && timer->id == id)
      return elapsedMs >= timer->durationMs;
  }
  return false;
}