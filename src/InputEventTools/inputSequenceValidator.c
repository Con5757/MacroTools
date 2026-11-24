#include "inputSequenceValidator.h"
#include <string.h>

BOOL inputSequenceValidatorValidateWithOptions(
    const InputEvent *events, size_t count,
    const InputSequenceValidationOptions *options,
    InputSequenceValidation *result) {
  if (result == NULL || (count != 0 && events == NULL))
    return FALSE;
  InputSequenceValidationOptions defaults = {TRUE, TRUE};
  if (options == NULL)
    options = &defaults;
  memset(result, 0, sizeof(*result));
  result->eventCount = count;
  BYTE keyDown[256] = {0};
  BYTE buttonDown[6] = {0};
  for (size_t i = 0; i < count; ++i) {
    const InputEvent *e = &events[i];
    if (i > 0 && e->timestampMs < events[i - 1].timestampMs)
      ++result->outOfOrderTimestamps;
    if ((e->type == INPUT_EVENT_KEY_DOWN || e->type == INPUT_EVENT_KEY_UP) &&
        e->virtualKey < 256) {
      BYTE *state = &keyDown[e->virtualKey];
      if (e->type == INPUT_EVENT_KEY_DOWN) {
        if (*state) {
          if (options->allowKeyRepeats)
            ++result->keyRepeatEvents;
          else
            ++result->duplicateKeyDowns;
        }
        *state = 1;
      } else {
        if (!*state)
          ++result->keyUpsWithoutDown;
        *state = 0;
      }
    } else if (e->type == INPUT_EVENT_MOUSE_BUTTON_DOWN ||
               e->type == INPUT_EVENT_MOUSE_BUTTON_UP) {
      if (e->mouseButton >= 1 && e->mouseButton <= 5) {
        BYTE *state = &buttonDown[e->mouseButton];
        if (e->type == INPUT_EVENT_MOUSE_BUTTON_DOWN) {
          if (*state)
            ++result->duplicateButtonDowns;
          *state = 1;
        } else {
          if (!*state)
            ++result->buttonUpsWithoutDown;
          *state = 0;
        }
      }
    }
  }
  for (size_t i = 0; i < 256; ++i)
    result->keysStillDown += keyDown[i] != 0;
  for (size_t i = 1; i <= 5; ++i)
    result->buttonsStillDown += buttonDown[i] != 0;
  result->valid =
      result->outOfOrderTimestamps == 0 && result->duplicateKeyDowns == 0 &&
      result->keyUpsWithoutDown == 0 && result->duplicateButtonDowns == 0 &&
      result->buttonUpsWithoutDown == 0 &&
      (!options->requireAllInputsReleased ||
       (result->keysStillDown == 0 && result->buttonsStillDown == 0));
  return TRUE;
}
BOOL inputSequenceValidatorValidate(const InputEvent *events, size_t count,
                                    InputSequenceValidation *result) {
  const InputSequenceValidationOptions defaults = {TRUE, TRUE};
  return inputSequenceValidatorValidateWithOptions(events, count, &defaults,
                                                   result);
}