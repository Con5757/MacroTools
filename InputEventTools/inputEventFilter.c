#include "inputEventFilter.h"
#include <string.h>

void inputEventFilterInitialize(InputEventFilter *filter) {
  if (filter != NULL)
    memset(filter, 0, sizeof(*filter));
}
BOOL inputEventFilterMatches(const InputEventFilter *filter,
                             const InputEvent *event) {
  if (filter == NULL || event == NULL || event->type < INPUT_EVENT_KEY_DOWN ||
      event->type > INPUT_EVENT_DEVICE_REMOVAL)
    return FALSE;
  if (filter->typeMask != 0 &&
      (filter->typeMask & (1u << (unsigned)event->type)) == 0)
    return FALSE;
  if (filter->deviceHandle != NULL &&
      filter->deviceHandle != event->deviceHandle)
    return FALSE;
  if (filter->virtualKey != 0 && filter->virtualKey != event->virtualKey)
    return FALSE;
  if (filter->mouseButton != 0 && filter->mouseButton != event->mouseButton)
    return FALSE;
  if (filter->source != INPUT_EVENT_SOURCE_UNKNOWN &&
      filter->source != event->source)
    return FALSE;
  return TRUE;
}
size_t inputEventFilterCopy(const InputEventFilter *filter,
                            const InputEvent *input, size_t inputCount,
                            InputEvent *output, size_t outputCapacity) {
  if (filter == NULL || (inputCount != 0 && input == NULL) ||
      (outputCapacity != 0 && output == NULL))
    return 0;
  size_t matched = 0;
  for (size_t i = 0; i < inputCount; ++i) {
    if (inputEventFilterMatches(filter, &input[i])) {
      if (matched < outputCapacity)
        output[matched] = input[i];
      ++matched;
    }
  }
  return matched;
}