#include "inputEventRecorder.h"
#include <string.h>

BOOL inputEventRecorderInitialize(InputEventRecorder *recorder,
                                  size_t initialCapacity) {
  if (recorder == NULL)
    return FALSE;
  memset(recorder, 0, sizeof(*recorder));
  return inputEventBufferInitialize(&recorder->buffer, initialCapacity);
}
void inputEventRecorderStart(InputEventRecorder *recorder, BOOL clearExisting) {
  if (recorder == NULL)
    return;
  if (clearExisting)
    inputEventBufferClear(&recorder->buffer);
  recorder->recording = TRUE;
}
void inputEventRecorderStop(InputEventRecorder *recorder) {
  if (recorder != NULL)
    recorder->recording = FALSE;
}
BOOL inputEventRecorderAppend(InputEventRecorder *recorder,
                              const InputEvent *event) {
  return recorder != NULL && recorder->recording &&
         inputEventBufferAppend(&recorder->buffer, event);
}
const InputEvent *
inputEventRecorderGetEvents(const InputEventRecorder *recorder, size_t *count) {
  if (count != NULL)
    *count = recorder == NULL ? 0 : recorder->buffer.count;
  return recorder == NULL ? NULL : recorder->buffer.events;
}
void inputEventRecorderDestroy(InputEventRecorder *recorder) {
  if (recorder == NULL)
    return;
  inputEventBufferDestroy(&recorder->buffer);
  recorder->recording = FALSE;
}