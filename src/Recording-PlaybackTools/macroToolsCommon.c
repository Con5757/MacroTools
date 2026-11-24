#include "macroToolsCommon.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static bool macroNameValid(const char *name) {
  return name != NULL && name[0] != '\0' && strlen(name) < MACRO_NAME_CAPACITY;
}
void macroStepInitialize(MacroStep *step, MacroStepType type) {
  if (step == NULL)
    return;
  memset(step, 0, sizeof(*step));
  step->type = (uint32_t)type;
  step->target_step = MACRO_NO_TARGET;
  step->alternate_step = MACRO_NO_TARGET;
}
void macroSequenceInitialize(MacroSequence *sequence, const char *name) {
  if (sequence == NULL)
    return;
  memset(sequence, 0, sizeof(*sequence));
  sequence->next_id = 1;
  if (name != NULL) {
    strncpy(sequence->name, name, MACRO_NAME_CAPACITY - 1);
    sequence->name[MACRO_NAME_CAPACITY - 1] = '\0';
  }
}
void macroSequenceDestroy(MacroSequence *sequence) {
  if (sequence == NULL)
    return;
  free(sequence->steps);
  memset(sequence, 0, sizeof(*sequence));
}
bool macroSequenceReserve(MacroSequence *sequence, size_t capacity) {
  if (sequence == NULL)
    return false;
  if (capacity <= sequence->capacity)
    return true;
  if (capacity > SIZE_MAX / sizeof(MacroStep))
    return false;
  size_t next = sequence->capacity == 0 ? 8 : sequence->capacity;
  while (next < capacity) {
    if (next > SIZE_MAX / 2) {
      next = capacity;
      break;
    }
    next *= 2;
  }
  if (next > SIZE_MAX / sizeof(MacroStep))
    return false;
  MacroStep *items =
      (MacroStep *)realloc(sequence->steps, next * sizeof(MacroStep));
  if (items == NULL)
    return false;
  sequence->steps = items;
  sequence->capacity = next;
  return true;
}
bool macroSequenceInsert(MacroSequence *sequence, size_t index,
                         const MacroStep *step) {
  if (sequence == NULL || step == NULL || index > sequence->count)
    return false;
  if (sequence->count == SIZE_MAX ||
      !macroSequenceReserve(sequence, sequence->count + 1))
    return false;
  memmove(&sequence->steps[index + 1], &sequence->steps[index],
          (sequence->count - index) * sizeof(MacroStep));
  sequence->steps[index] = *step;
  if (sequence->steps[index].id == 0)
    sequence->steps[index].id = sequence->next_id++;
  else if (sequence->steps[index].id >= sequence->next_id)
    sequence->next_id = sequence->steps[index].id + 1;
  sequence->count++;
  return true;
}
bool macroSequenceAppend(MacroSequence *sequence, const MacroStep *step) {
  return sequence != NULL &&
         macroSequenceInsert(sequence, sequence->count, step);
}

bool macroSequenceRemove(MacroSequence *sequence, size_t index) {
  if (sequence == NULL || index >= sequence->count)
    return false;
  if (index + 1 < sequence->count) {
    memmove(&sequence->steps[index], &sequence->steps[index + 1],
            (sequence->count - index - 1) * sizeof(MacroStep));
  }
  sequence->count--;
  return true;
}
bool macroSequenceClone(MacroSequence *destination,
                        const MacroSequence *source) {
  if (destination == NULL || source == NULL || destination == source)
    return false;
  MacroSequence copy;
  macroSequenceInitialize(&copy, source->name);
  if (source->count > 0) {
    if (!macroSequenceReserve(&copy, source->count))
      return false;
    memcpy(copy.steps, source->steps, source->count * sizeof(MacroStep));
    copy.count = source->count;
  }
  copy.next_id = source->next_id;
  macroSequenceDestroy(destination);
  *destination = copy;
  return true;
}
MacroStep *macroSequenceGet(MacroSequence *sequence, size_t index) {
  return sequence != NULL && index < sequence->count ? &sequence->steps[index]
                                                     : NULL;
}
const MacroStep *macroSequenceGetConst(const MacroSequence *sequence,
                                       size_t index) {
  return sequence != NULL && index < sequence->count ? &sequence->steps[index]
                                                     : NULL;
}
void macroVariableStoreInitialize(MacroVariableStore *store) {
  if (store != NULL)
    memset(store, 0, sizeof(*store));
}
void macroVariableStoreDestroy(MacroVariableStore *store) {
  if (store == NULL)
    return;
  free(store->items);
  memset(store, 0, sizeof(*store));
}
bool macroVariableStoreSet(MacroVariableStore *store, const char *name,
                           double value) {
  if (store == NULL || !macroNameValid(name) || !isfinite(value))
    return false;
  for (size_t i = 0; i < store->count; ++i) {
    if (strcmp(store->items[i].name, name) == 0) {
      store->items[i].value = value;
      return true;
    }
  }
  if (store->count == store->capacity) {
    size_t cap = store->capacity == 0 ? 8 : store->capacity * 2;
    if (cap < store->capacity || cap > SIZE_MAX / sizeof(MacroVariable))
      return false;
    MacroVariable *items =
        (MacroVariable *)realloc(store->items, cap * sizeof(MacroVariable));
    if (items == NULL)
      return false;
    store->items = items;
    store->capacity = cap;
  }
  MacroVariable *v = &store->items[store->count++];
  strncpy(v->name, name, sizeof(v->name) - 1);
  v->name[sizeof(v->name) - 1] = '\0';
  v->value = value;
  return true;
}
bool macroVariableStoreGet(const MacroVariableStore *store, const char *name,
                           double *value) {
  if (store == NULL || name == NULL || value == NULL)
    return false;
  for (size_t i = 0; i < store->count; ++i) {
    if (strcmp(store->items[i].name, name) == 0) {
      *value = store->items[i].value;
      return true;
    }
  }
  return false;
}
bool macroVariableStoreRemove(MacroVariableStore *store, const char *name) {
  if (store == NULL || name == NULL)
    return false;
  for (size_t i = 0; i < store->count; ++i) {
    if (strcmp(store->items[i].name, name) == 0) {
      if (i + 1 < store->count)
        memmove(&store->items[i], &store->items[i + 1],
                (store->count - i - 1) * sizeof(MacroVariable));
      store->count--;
      return true;
    }
  }
  return false;
}
bool macroConditionEvaluate(double left, MacroConditionOperator op,
                            double right, bool *result) {
  if (result == NULL || !isfinite(left) || !isfinite(right))
    return false;
  switch (op) {
  case MACRO_CONDITION_EQUAL:
    *result = left == right;
    break;
  case MACRO_CONDITION_NOT_EQUAL:
    *result = left != right;
    break;
  case MACRO_CONDITION_GREATER:
    *result = left > right;
    break;
  case MACRO_CONDITION_GREATER_EQUAL:
    *result = left >= right;
    break;
  case MACRO_CONDITION_LESS:
    *result = left < right;
    break;
  case MACRO_CONDITION_LESS_EQUAL:
    *result = left <= right;
    break;
  case MACRO_CONDITION_TRUTHY:
    *result = left != 0.0;
    break;
  case MACRO_CONDITION_FALSEY:
    *result = left == 0.0;
    break;
  default:
    return false;
  }
  return true;
}
bool macroConditionEvaluateStep(const MacroStep *step,
                                const MacroVariableStore *variables,
                                bool *result) {
  double left;
  if (step == NULL || variables == NULL || result == NULL ||
      !macroVariableStoreGet(variables, step->text, &left))
    return false;
  return macroConditionEvaluate(left, (MacroConditionOperator)step->code,
                                step->number, result);
}
bool macroPlaybackPause(MacroPlaybackState *state, uint64_t now_ms) {
  if (state == NULL || !state->active || state->paused)
    return false;
  state->paused = true;
  state->pause_started_ms = now_ms;
  return true;
}
bool macroPlaybackResume(MacroPlaybackState *state, uint64_t now_ms) {
  if (state == NULL || !state->active || !state->paused)
    return false;
  uint64_t pause_delta =
      now_ms >= state->pause_started_ms ? now_ms - state->pause_started_ms : 0;
  if (UINT64_MAX - state->due_at_ms < pause_delta)
    state->due_at_ms = UINT64_MAX;
  else
    state->due_at_ms += pause_delta;
  state->paused = false;
  return true;
}
void macroPlaybackRequestStop(MacroPlaybackState *state) {
  if (state != NULL)
    state->stop_requested = true;
}
bool macroPlaybackSetSpeed(MacroPlaybackState *state, double speed) {
  if (state == NULL || !isfinite(speed) || speed < 0.05 || speed > 20.0)
    return false;
  state->speed = speed;
  return true;
}