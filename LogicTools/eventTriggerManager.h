#pragma once
#include "logicToolsCommon.h"

typedef void (*LAEventCallback)(void *context, uint32_t eventId, double value);
typedef struct {
  uint32_t eventId;
  LAEventCallback callback;
  void *context;
  bool occupied;
} LAEventSubscription;
typedef struct {
  LAEventSubscription subscriptions[LA_MAX_ITEMS];
} LAEventTriggerManager;

void laEventTriggerInitialize(LAEventTriggerManager *manager);
bool laEventSubscribe(LAEventTriggerManager *manager, uint32_t eventId,
                      LAEventCallback callback, void *context);
bool laEventUnsubscribe(LAEventTriggerManager *manager, uint32_t eventId,
                        LAEventCallback callback, void *context);
size_t laEventDispatch(LAEventTriggerManager *manager, uint32_t eventId,
                       double value);