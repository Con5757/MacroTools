#include "actionQueueManager.h"
#include <string.h>

void laActionQueueInitialize(LAActionQueue *q) {
  if (q)
    memset(q, 0, sizeof(*q));
}
bool laActionQueuePush(LAActionQueue *q, LAActionCallback cb, void *ctx,
                       uint32_t id, double value) {
  if (!q || !cb || q->count >= LA_MAX_ITEMS)
    return false;
  size_t tail = (q->head + q->count) % LA_MAX_ITEMS;
  q->items[tail] = (LAQueuedAction){cb, ctx, id, value};
  q->count++;
  return true;
}
bool laActionQueuePop(LAActionQueue *q, LAQueuedAction *out) {
  if (!q || !out || q->count == 0)
    return false;
  *out = q->items[q->head];
  memset(&q->items[q->head], 0, sizeof(q->items[q->head]));
  q->head = (q->head + 1) % LA_MAX_ITEMS;
  q->count--;
  return true;
}
size_t laActionQueueProcess(LAActionQueue *q, size_t max) {
  if (!q)
    return 0;
  size_t done = 0;
  LAQueuedAction a;
  while (done < max && laActionQueuePop(q, &a)) {
    a.callback(a.context, a.actionId, a.value);
    done++;
  }
  return done;
}
size_t laActionQueueCount(const LAActionQueue *q) { return q ? q->count : 0; }