#include "cooldownManager.h"
#include <string.h>

void laCooldownManagerInitialize(LACooldownManager *m) {
  if (m)
    memset(m, 0, sizeof(*m));
}
static LACooldownEntry *get(LACooldownManager *m, uint32_t id, bool create) {
  LACooldownEntry *f = NULL;
  if (!m)
    return NULL;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++) {
    if (m->entries[i].occupied && m->entries[i].actionId == id)
      return &m->entries[i];
    if (!m->entries[i].occupied && !f)
      f = &m->entries[i];
  }
  if (create && f) {
    f->occupied = true;
    f->actionId = id;
    return f;
  }
  return NULL;
}
bool laCooldownConfigure(LACooldownManager *m, uint32_t id, uint64_t ms) {
  LACooldownEntry *e = get(m, id, true);
  if (!e)
    return false;
  e->cooldownMs = ms;
  return true;
}
bool laCooldownReady(const LACooldownManager *m, uint32_t id, uint64_t now) {
  if (!m)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++) {
    const LACooldownEntry *e = &m->entries[i];
    if (e->occupied && e->actionId == id) {
      return !e->hasUsed || now < e->lastUseMs ||
             now - e->lastUseMs >= e->cooldownMs;
    }
  }
  return true;
}
bool laCooldownTryAcquire(LACooldownManager *m, uint32_t id, uint64_t now) {
  if (!m || !laCooldownReady(m, id, now))
    return false;
  LACooldownEntry *e = get(m, id, true);
  if (!e)
    return false;
  e->lastUseMs = now;
  e->hasUsed = true;
  return true;
}