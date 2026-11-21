#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "keyboardToolsCommon.h"
#include <Windows.h>
#include <stddef.h>

typedef struct {
  KeyboardRecordedEvent *events;
  size_t eventCount;
  size_t capacity;
  HHOOK hook;
  BOOL isRecording;
  BOOL allocationFailed;
} KeyRecorder;

BOOL keyRecorderInitialize(KeyRecorder *recorder);
BOOL keyRecorderStart(KeyRecorder *recorder);
void keyRecorderStop(KeyRecorder *recorder);
void keyRecorderClear(KeyRecorder *recorder);
void keyRecorderDestroy(KeyRecorder *recorder);
const KeyboardRecordedEvent *keyRecorderGetEvents(const KeyRecorder *recorder,
                                                  size_t *eventCount);
BOOL keyRecorderIsRecording(const KeyRecorder *recorder);