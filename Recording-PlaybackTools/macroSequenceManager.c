#include "macroSequenceManager.h"
#include <stdlib.h>
#include <string.h>

void macroSequenceManagerInitialize(MacroSequenceManager *manager) {
  if (manager != NULL) {
    memset(manager, 0, sizeof(*manager));
  }
}
void macroSequenceManagerDestroy(MacroSequenceManager *manager) {
  if (manager == NULL) {
    return;
  }
  free(manager->items);
  memset(manager, 0, sizeof(*manager));
}
bool macroSequenceManagerAdd(MacroSequenceManager *manager,
                             MacroSequence *sequence) {
  if (manager == NULL || sequence == NULL || sequence->name[0] == '\0') {
    return false;
  }
  for (size_t i = 0; i < manager->count; ++i) {
    if (manager->items[i] == sequence ||
        strcmp(manager->items[i]->name, sequence->name) == 0) {
      return false;
    }
  }
  if (manager->count == manager->capacity) {
    size_t capacity = manager->capacity == 0 ? 8 : manager->capacity * 2;
    if (capacity < manager->capacity ||
        capacity > SIZE_MAX / sizeof(*manager->items)) {
      return false;
    }
    MacroSequence **items = (MacroSequence **)realloc(
        manager->items, capacity * sizeof(*manager->items));
    if (items == NULL) {
      return false;
    }
    manager->items = items;
    manager->capacity = capacity;
  }
  manager->items[manager->count++] = sequence;
  return true;
}
bool macroSequenceManagerRemove(MacroSequenceManager *manager,
                                const MacroSequence *sequence) {
  if (manager == NULL || sequence == NULL) {
    return false;
  }
  for (size_t i = 0; i < manager->count; ++i) {
    if (manager->items[i] == sequence) {
      if (i + 1 < manager->count) {
        memmove(&manager->items[i], &manager->items[i + 1],
                (manager->count - i - 1) * sizeof(*manager->items));
      }
      manager->count--;
      return true;
    }
  }
  return false;
}
MacroSequence *macroSequenceManagerFind(MacroSequenceManager *manager,
                                        const char *name) {
  if (manager == NULL || name == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < manager->count; ++i) {
    if (strcmp(manager->items[i]->name, name) == 0) {
      return manager->items[i];
    }
  }
  return NULL;
}
