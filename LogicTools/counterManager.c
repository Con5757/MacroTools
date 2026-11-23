#include "counterManager.h"
#include <limits.h>
#include <string.h>

static LACounter *findCounter(LACounterManager *m, uint32_t id, bool create) {
  LACounter *freeSlot = NULL;
  if (!m)
    return NULL;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++) {
    if (m->counters[i].occupied && m->counters[i].id == id)
      return &m->counters[i];
    if (!m->counters[i].occupied && !freeSlot)
      freeSlot = &m->counters[i];
  }
  if (create && freeSlot) {
    freeSlot->occupied = true;
    freeSlot->id = id;
    freeSlot->value = 0;
    return freeSlot;
  }
  return NULL;
}
void laCounterManagerInitialize(LACounterManager *m) {
  if (m)
    memset(m, 0, sizeof(*m));
}
bool laCounterSet(LACounterManager *m, uint32_t id, int64_t v) {
  LACounter *c = findCounter(m, id, true);
  if (!c)
    return false;
  c->value = v;
  return true;
}
bool laCounterGet(const LACounterManager *m, uint32_t id, int64_t *v) {
  if (!m || !v)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++)
    if (m->counters[i].occupied && m->counters[i].id == id) {
      *v = m->counters[i].value;
      return true;
    }
  return false;
}
bool laCounterAdd(LACounterManager *m, uint32_t id, int64_t amount,
                  int64_t *out) {
  LACounter *c = findCounter(m, id, true);
  if (!c)
    return false;
  if ((amount > 0 && c->value > INT64_MAX - amount) ||
      (amount < 0 && c->value < INT64_MIN - amount))
    return false;
  c->value += amount;
  if (out)
    *out = c->value;
  return true;
}
bool laCounterIncrement(LACounterManager *m, uint32_t id) {
  return laCounterAdd(m, id, 1, NULL);
}
bool laCounterRemove(LACounterManager *m, uint32_t id) {
  LACounter *c = findCounter(m, id, false);
  if (!c)
    return false;
  memset(c, 0, sizeof(*c));
  return true;
}