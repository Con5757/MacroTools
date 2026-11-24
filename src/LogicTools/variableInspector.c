#include "variableInspector.h"
#include <string.h>

static LAVariable *findVar(LAVariableStore *s, const char *n, bool create) {
  LAVariable *freeSlot = NULL;
  if (!s || !n)
    return NULL;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++) {
    if (s->variables[i].occupied && strcmp(s->variables[i].name, n) == 0)
      return &s->variables[i];
    if (!s->variables[i].occupied && !freeSlot)
      freeSlot = &s->variables[i];
  }
  if (create && freeSlot && laCopyName(freeSlot->name, n)) {
    freeSlot->occupied = true;
    return freeSlot;
  }
  return NULL;
}
void laVariableStoreInitialize(LAVariableStore *s) {
  if (s)
    memset(s, 0, sizeof(*s));
}
bool laVariableSetNumber(LAVariableStore *s, const char *n, double v) {
  LAVariable *x = findVar(s, n, true);
  if (!x)
    return false;
  x->value.type = LA_VALUE_NUMBER;
  x->value.data.number = v;
  return true;
}
bool laVariableSetBoolean(LAVariableStore *s, const char *n, bool v) {
  LAVariable *x = findVar(s, n, true);
  if (!x)
    return false;
  x->value.type = LA_VALUE_BOOLEAN;
  x->value.data.boolean = v;
  return true;
}
bool laVariableGet(const LAVariableStore *s, const char *n, LAValue *v) {
  if (!s || !n || !v)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++)
    if (s->variables[i].occupied && strcmp(s->variables[i].name, n) == 0) {
      *v = s->variables[i].value;
      return true;
    }
  return false;
}
bool laVariableRemove(LAVariableStore *s, const char *n) {
  if (!s || !n)
    return false;
  for (size_t i = 0; i < LA_MAX_ITEMS; i++)
    if (s->variables[i].occupied && strcmp(s->variables[i].name, n) == 0) {
      memset(&s->variables[i], 0, sizeof(s->variables[i]));
      return true;
    }
  return false;
}
bool laVariableResolveNumber(void *ctx, const char *n, double *out) {
  LAValue v;
  if (!out || !laVariableGet((const LAVariableStore *)ctx, n, &v))
    return false;
  *out =
      v.type == LA_VALUE_BOOLEAN ? (v.data.boolean ? 1.0 : 0.0) : v.data.number;
  return true;
}