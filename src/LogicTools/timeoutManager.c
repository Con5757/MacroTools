#include "timeoutManager.h"
#include <string.h>

void laTimeoutManagerInitialize(LATimeoutManager *manager) {
  if (manager != NULL)
    memset(manager, 0, sizeof(*manager));
}
static LATimeout *findTimeout(LATimeoutManager *manager, uint32_t id,
                              bool create) {
  LATimeout *freeSlot = NULL;
  if (manager == NULL)
    return NULL;
  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    LATimeout *timeout = &manager->timeouts[i];
    if (timeout->active && timeout->id == id)
      return timeout;
    if (!timeout->active && freeSlot == NULL)
      freeSlot = timeout;
  }
  if (create && freeSlot != NULL)
    return freeSlot;
  return NULL;
}
bool laTimeoutStart(LATimeoutManager *manager, uint32_t id, uint64_t nowMs,
                    uint64_t durationMs) {
  LATimeout *timeout = findTimeout(manager, id, true);
  if (timeout == NULL)
    return false;
  timeout->id = id;
  timeout->startMs = nowMs;
  timeout->durationMs = durationMs;
  timeout->active = true;
  return true;
}
bool laTimeoutCancel(LATimeoutManager *manager, uint32_t id) {
  if (manager == NULL)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    LATimeout *timeout = &manager->timeouts[i];
    if (timeout->active && timeout->id == id) {
      timeout->active = false;
      return true;
    }
  }
  return false;
}
bool laTimeoutExpired(const LATimeoutManager *manager, uint32_t id,
                      uint64_t nowMs) {
  bool found = false;
  uint64_t remaining = laTimeoutRemaining(manager, id, nowMs, &found);
  return found && remaining == 0;
}
uint64_t laTimeoutRemaining(const LATimeoutManager *manager, uint32_t id,
                            uint64_t nowMs, bool *found) {
  if (found != NULL)
    *found = false;
  if (manager == NULL)
    return 0;
  for (size_t i = 0; i < LA_MAX_ITEMS; ++i) {
    const LATimeout *timeout = &manager->timeouts[i];
    if (timeout->active && timeout->id == id) {
      if (found != NULL)
        *found = true;
      uint64_t elapsed =
          nowMs >= timeout->startMs ? nowMs - timeout->startMs : 0;
      return elapsed >= timeout->durationMs ? 0 : timeout->durationMs - elapsed;
    }
  }
  return 0;
}