#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum InputConflictKind {
  INPUT_CONFLICT_DUPLICATE_KEY_DOWN = 1,
  INPUT_CONFLICT_KEY_UP_WITHOUT_DOWN,
  INPUT_CONFLICT_DUPLICATE_BUTTON_DOWN,
  INPUT_CONFLICT_BUTTON_UP_WITHOUT_DOWN,
  INPUT_CONFLICT_UNKNOWN_SOURCE,
  INPUT_CONFLICT_MISSING_DEVICE
} InputConflictKind;
typedef struct InputConflict {
  size_t eventIndex;
  InputConflictKind kind;
  UINT code;
} InputConflict;

size_t inputConflictDetectorAnalyze(
    const InputEvent *events, size_t count, InputConflict *output,
    size_t outputCapacity); // writes most of what given

#ifdef __cplusplus
}
#endif