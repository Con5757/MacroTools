#include "eventTriggerManager.h"
#include <string.h>

void laEventTriggerInitialize(LAEventTriggerManager *m) {
  if (m)
    memset(m, 0, sizeof(*m));
}
bool laEventSubscribe(LAEventTriggerManager *m, uint32_t id, LAEventCallback cb,
                      void *ctx) {
  if (!m || !cb)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++)
    if (!m->subscriptions[i].occupied) {
      m->subscriptions[i] = (LAEventSubscription){id, cb, ctx, true};
      return true;
    }
  return false;
}
bool laEventUnsubscribe(LAEventTriggerManager *m, uint32_t id,
                        LAEventCallback cb, void *ctx) {
  if (!m || !cb)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++) {
    LAEventSubscription *s = &m->subscriptions[i];
    if (s->occupied && s->eventId == id && s->callback == cb &&
        s->context == ctx) {
      memset(s, 0, sizeof(*s));
      return true;
    }
  }
  return false;
}
size_t laEventDispatch(LAEventTriggerManager *m, uint32_t id, double value) {
  if (!m)
    return 0;
  size_t count = 0;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++) {
    LAEventSubscription s = m->subscriptions[i];
    if (s.occupied && s.eventId == id) {
      s.callback(s.context, id, value);
      count++;
    }
  }
  return count;
}