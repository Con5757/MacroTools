#pragma once
#include "logicToolsCommon.h"

typedef struct {
  char name[LA_NAME_CAPACITY];
  LAValue value;
  bool occupied;
} LAVariable;
typedef struct {
  LAVariable variables[LA_MAX_ITEMS];
} LAVariableStore;

void laVariableStoreInitialize(LAVariableStore *store);
bool laVariableSetNumber(LAVariableStore *store, const char *name,
                         double value);
bool laVariableSetBoolean(LAVariableStore *store, const char *name, bool value);
bool laVariableGet(const LAVariableStore *store, const char *name,
                   LAValue *value);
bool laVariableRemove(LAVariableStore *store, const char *name);
bool laVariableResolveNumber(void *store, const char *name,
                             double *value); // 0 or 1