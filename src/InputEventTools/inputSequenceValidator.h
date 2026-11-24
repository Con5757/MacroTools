#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputSequenceValidation {
  size_t eventCount;
  size_t outOfOrderTimestamps;
  size_t duplicateKeyDowns;
  size_t keyRepeatEvents;
  size_t keyUpsWithoutDown;
  size_t duplicateButtonDowns;
  size_t buttonUpsWithoutDown;
  size_t keysStillDown;
  size_t buttonsStillDown;
  BOOL valid;
} InputSequenceValidation;
typedef struct InputSequenceValidationOptions {
  BOOL allowKeyRepeats;
  BOOL requireAllInputsReleased;
} InputSequenceValidationOptions;

BOOL inputSequenceValidatorValidateWithOptions(
    const InputEvent *events, size_t count,
    const InputSequenceValidationOptions *options,
    InputSequenceValidation *result);
BOOL inputSequenceValidatorValidate(const InputEvent *events, size_t count,
                                    InputSequenceValidation *result);

#ifdef __cplusplus
}
#endif
