#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputEventRecorder {
  InputEventBuffer buffer;
  BOOL recording;
} InputEventRecorder;

BOOL inputEventRecorderInitialize(InputEventRecorder *recorder,
                                  size_t initialCapacity);
void inputEventRecorderStart(InputEventRecorder *recorder, BOOL clearExisting);
void inputEventRecorderStop(InputEventRecorder *recorder);
BOOL inputEventRecorderAppend(InputEventRecorder *recorder,
                              const InputEvent *event);
const InputEvent *
inputEventRecorderGetEvents(const InputEventRecorder *recorder, size_t *count);
void inputEventRecorderDestroy(InputEventRecorder *recorder);

#ifdef __cplusplus
}
#endif