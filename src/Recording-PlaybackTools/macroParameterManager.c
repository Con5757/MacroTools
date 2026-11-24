#include "macroParameterManager.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void macroParameterManagerInitialize(MacroParameterManager *m) {
  if (m)
    memset(m, 0, sizeof(*m));
}
void macroParameterManagerDestroy(MacroParameterManager *m) {
  if (!m)
    return;
  free(m->items);
  memset(m, 0, sizeof(*m));
}
static MacroParameter *findParam(MacroParameterManager *m, const char *n) {
  if (!m || !n)
    return NULL;
  for (size_t i = 0; i < m->count; i++)
    if (strcmp(m->items[i].name, n) == 0)
      return &m->items[i];
  return NULL;
}
bool macroParameterManagerDeclare(MacroParameterManager *m, const char *n,
                                  double d) {
  if (!m || !n || !*n || strlen(n) >= MACRO_NAME_CAPACITY || !isfinite(d) ||
      findParam(m, n))
    return false;
  if (m->count == m->capacity) {
    size_t c = m->capacity ? m->capacity * 2 : 8;
    if (c < m->capacity || c > SIZE_MAX / sizeof(*m->items))
      return false;
    void *p = realloc(m->items, c * sizeof(*m->items));
    if (!p)
      return false;
    m->items = p;
    m->capacity = c;
  }
  MacroParameter *p = &m->items[m->count++];
  memset(p, 0, sizeof(*p));
  strcpy(p->name, n);
  p->default_value = d;
  p->value = d;
  return true;
}
bool macroParameterManagerSet(MacroParameterManager *m, const char *n,
                              double v) {
  MacroParameter *p = findParam(m, n);
  if (!p || !isfinite(v))
    return false;
  p->value = v;
  p->has_override = true;
  return true;
}
bool macroParameterManagerGet(const MacroParameterManager *m, const char *n,
                              double *v) {
  if (!m || !n || !v)
    return false;
  for (size_t i = 0; i < m->count; i++)
    if (strcmp(m->items[i].name, n) == 0) {
      *v = m->items[i].value;
      return true;
    }
  return false;
}
void macroParameterManagerResetOverrides(MacroParameterManager *m) {
  if (!m)
    return;
  for (size_t i = 0; i < m->count; i++) {
    m->items[i].value = m->items[i].default_value;
    m->items[i].has_override = false;
  }
}