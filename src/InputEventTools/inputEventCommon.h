#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum InputEventType {
  INPUT_EVENT_KEY_DOWN = 1,
  INPUT_EVENT_KEY_UP,
  INPUT_EVENT_MOUSE_MOVE,
  INPUT_EVENT_MOUSE_BUTTON_DOWN,
  INPUT_EVENT_MOUSE_BUTTON_UP,
  INPUT_EVENT_MOUSE_WHEEL,
  INPUT_EVENT_DEVICE_ARRIVAL,
  INPUT_EVENT_DEVICE_REMOVAL
} InputEventType;
typedef enum InputEventSource {
  INPUT_EVENT_SOURCE_UNKNOWN = 0,
  INPUT_EVENT_SOURCE_PHYSICAL,
  INPUT_EVENT_SOURCE_INJECTED
} InputEventSource;
typedef struct InputEvent {
  ULONGLONG timestampMs;
  DWORD messageTime;
  InputEventType type;
  InputEventSource source;
  HANDLE deviceHandle;
  UINT virtualKey;
  UINT scanCode;
  USHORT rawKeyboardFlags;
  USHORT rawMouseFlags;
  USHORT rawButtonFlags;
  USHORT rawButtonData;
  UINT mouseButton; // 1: left, 2: right, 3: middle, 4: X1, 5: X2
  LONG x; // raw coord data/relative
  LONG y;
  LONG wheelDelta;
  DWORD extraInfo;
} InputEvent;
typedef struct InputEventBuffer {
  InputEvent *events;
  size_t count;
  size_t capacity;
} InputEventBuffer;

BOOL inputEventBufferInitialize(InputEventBuffer *buffer,
                                size_t initialCapacity);
BOOL inputEventBufferAppend(InputEventBuffer *buffer, const InputEvent *event);
void inputEventBufferClear(InputEventBuffer *buffer);
void inputEventBufferDestroy(InputEventBuffer *buffer);
const char *inputEventTypeName(InputEventType type);
const char *inputEventSourceName(InputEventSource source);
InputEventSource inputEventClassifySource(HANDLE deviceHandle, BOOL injected);

#ifdef __cplusplus
}
#endif