#include "inputEventCommon.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

BOOL inputEventBufferInitialize(InputEventBuffer *buffer,
                                size_t initialCapacity) {
  if (buffer == NULL)
    return FALSE;
  memset(buffer, 0, sizeof(*buffer));
  if (initialCapacity == 0)
    return TRUE;
  if (initialCapacity > SIZE_MAX / sizeof(InputEvent))
    return FALSE;
  buffer->events = (InputEvent *)malloc(initialCapacity * sizeof(InputEvent));
  if (buffer->events == NULL)
    return FALSE;
  buffer->capacity = initialCapacity;
  return TRUE;
}
BOOL inputEventBufferAppend(InputEventBuffer *buffer, const InputEvent *event) {
  if (buffer == NULL || event == NULL)
    return FALSE;
  if (buffer->count == buffer->capacity) {
    size_t newCapacity = buffer->capacity == 0 ? 32 : buffer->capacity * 2;
    if (newCapacity < buffer->capacity ||
        newCapacity > SIZE_MAX / sizeof(InputEvent))
      return FALSE;
    InputEvent *newEvents =
        (InputEvent *)realloc(buffer->events, newCapacity * sizeof(InputEvent));
    if (newEvents == NULL)
      return FALSE;
    buffer->events = newEvents;
    buffer->capacity = newCapacity;
  }
  buffer->events[buffer->count++] = *event;
  return TRUE;
}
void inputEventBufferClear(InputEventBuffer *buffer) {
  if (buffer != NULL)
    buffer->count = 0;
}
void inputEventBufferDestroy(InputEventBuffer *buffer) {
  if (buffer == NULL)
    return;
  free(buffer->events);
  memset(buffer, 0, sizeof(*buffer));
}
const char *inputEventTypeName(InputEventType type) {
  switch (type) {
  case INPUT_EVENT_KEY_DOWN:
    return "key_down";
  case INPUT_EVENT_KEY_UP:
    return "key_up";
  case INPUT_EVENT_MOUSE_MOVE:
    return "mouse_move";
  case INPUT_EVENT_MOUSE_BUTTON_DOWN:
    return "mouse_button_down";
  case INPUT_EVENT_MOUSE_BUTTON_UP:
    return "mouse_button_up";
  case INPUT_EVENT_MOUSE_WHEEL:
    return "mouse_wheel";
  case INPUT_EVENT_DEVICE_ARRIVAL:
    return "device_arrival";
  case INPUT_EVENT_DEVICE_REMOVAL:
    return "device_removal";
  default:
    return "unknown";
  }
}
const char *inputEventSourceName(InputEventSource source) {
  switch (source) {
  case INPUT_EVENT_SOURCE_PHYSICAL:
    return "physical";
  case INPUT_EVENT_SOURCE_INJECTED:
    return "injected";
  default:
    return "unknown";
  }
}
InputEventSource inputEventClassifySource(HANDLE deviceHandle, BOOL injected) {
  if (injected)
    return INPUT_EVENT_SOURCE_INJECTED;
  if (deviceHandle != NULL)
    return INPUT_EVENT_SOURCE_PHYSICAL;
  return INPUT_EVENT_SOURCE_UNKNOWN;
}