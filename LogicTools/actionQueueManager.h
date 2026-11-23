#pragma once
#include "logicToolsCommon.h"

typedef struct {
  LAActionCallback callback;
  void *context;
  uint32_t actionId;
  double value;
} LAQueuedAction;
typedef struct {
  LAQueuedAction items[LA_MAX_ITEMS];
  size_t head;
  size_t count;
} LAActionQueue;

void laActionQueueInitialize(LAActionQueue *queue);
bool laActionQueuePush(LAActionQueue *queue, LAActionCallback callback,
                       void *context, uint32_t actionId, double value);
size_t laActionQueueProcess(LAActionQueue *queue, size_t maxActions);
bool laActionQueuePop(LAActionQueue *queue, LAQueuedAction *action);
size_t laActionQueueCount(const LAActionQueue *queue);