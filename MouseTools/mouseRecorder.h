#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <stddef.h>

typedef struct {
  POINT position;
  ULONGLONG timestampMs;
} MouseRecorderSample;

typedef enum {
  MOUSE_RECORDER_ERROR = -1,
  MOUSE_RECORDER_NO_SAMPLE = 0,
  MOUSE_RECORDER_SAMPLE_RECORDED = 1
} MouseRecorderUpdateResult;

typedef struct {
  MouseRecorderSample *samples;
  size_t sampleCount;
  size_t sampleCapacity;

  DWORD samplingIntervalMs;
  ULONGLONG startTimeMs;
  ULONGLONG lastPollTimeMs;

  POINT lastPosition;
  BOOL hasLastPosition;
  BOOL hasPolled;
  BOOL isRecording;
} MouseRecorder;

BOOL mouseRecorderInitialize(MouseRecorder *recorder, DWORD samplingIntervalMs);
BOOL mouseRecorderStart(MouseRecorder *recorder);
MouseRecorderUpdateResult mouseRecorderUpdate(MouseRecorder *recorder);
void mouseRecorderStop(MouseRecorder *recorder);
void mouseRecorderClear(MouseRecorder *recorder);
void mouseRecorderDestroy(MouseRecorder *recorder);
const MouseRecorderSample *
mouseRecorderGetSamples(const MouseRecorder *recorder, size_t *sampleCount);