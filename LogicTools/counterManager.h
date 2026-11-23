#pragma once
#include "logicToolsCommon.h"

typedef struct {
  uint32_t id;
  int64_t value;
  bool occupied;
} LACounter;
typedef struct {
  LACounter counters[LA_MAX_ITEMS];
} LACounterManager;

void laCounterManagerInitialize(LACounterManager *manager);
bool laCounterSet(LACounterManager *manager, uint32_t id, int64_t value);
bool laCounterGet(const LACounterManager *manager, uint32_t id, int64_t *value);
bool laCounterAdd(LACounterManager *manager, uint32_t id, int64_t amount,
                  int64_t *newValue);
bool laCounterIncrement(LACounterManager *manager, uint32_t id);
bool laCounterRemove(LACounterManager *manager, uint32_t id);