#define WIN32_LEAN_AND_MEAN
#include "mouseRecorder.h"

#include <stdint.h>
#include <stdlib.h>

#define MOUSE_RECORDER_MIN_INTERVAL_MS 1u
#define MOUSE_RECORDER_MAX_INTERVAL_MS 60000u
#define MOUSE_RECORDER_INITIAL_CAPACITY 256u

static BOOL mouseRecorderReserve(MouseRecorder *recorder,
                                 size_t requiredCount) {
  if (recorder == NULL) {
    return FALSE;
  }

  if (requiredCount <= recorder->sampleCapacity) {
    return TRUE;
  }

  size_t newCapacity = recorder->sampleCapacity;

  if (newCapacity == 0) {
    newCapacity = MOUSE_RECORDER_INITIAL_CAPACITY;
  }

  while (newCapacity < requiredCount) {
    if (newCapacity > SIZE_MAX / 2) {
      newCapacity = requiredCount;
      break;
    }

    newCapacity *= 2;
  }

  if (newCapacity > SIZE_MAX / sizeof(*recorder->samples)) {
    return FALSE;
  }

  MouseRecorderSample *newSamples = (MouseRecorderSample *)realloc(
      recorder->samples, newCapacity * sizeof(*recorder->samples));

  if (newSamples == NULL) {
    return FALSE;
  }

  recorder->samples = newSamples;
  recorder->sampleCapacity = newCapacity;

  return TRUE;
}

static BOOL mouseRecorderPositionChanged(POINT first, POINT second) {
  return first.x != second.x || first.y != second.y;
}

BOOL mouseRecorderInitialize(MouseRecorder *recorder,
                             DWORD samplingIntervalMs) {
  if (recorder == NULL || samplingIntervalMs < MOUSE_RECORDER_MIN_INTERVAL_MS ||
      samplingIntervalMs > MOUSE_RECORDER_MAX_INTERVAL_MS) {
    return FALSE;
  }

  ZeroMemory(recorder, sizeof(*recorder));
  recorder->samplingIntervalMs = samplingIntervalMs;

  return TRUE;
}

BOOL mouseRecorderStart(MouseRecorder *recorder) {
  if (recorder == NULL || recorder->samplingIntervalMs == 0) {
    return FALSE;
  }

  recorder->sampleCount = 0;
  recorder->startTimeMs = GetTickCount64();
  recorder->lastPollTimeMs = 0;

  recorder->lastPosition.x = 0;
  recorder->lastPosition.y = 0;

  recorder->hasLastPosition = FALSE;
  recorder->hasPolled = FALSE;
  recorder->isRecording = TRUE;

  return TRUE;
}

MouseRecorderUpdateResult mouseRecorderUpdate(MouseRecorder *recorder) {
  if (recorder == NULL) {
    return MOUSE_RECORDER_ERROR;
  }

  if (!recorder->isRecording) {
    return MOUSE_RECORDER_NO_SAMPLE;
  }

  ULONGLONG currentTimeMs = GetTickCount64();

  if (recorder->hasPolled &&
      currentTimeMs - recorder->lastPollTimeMs < recorder->samplingIntervalMs) {
    return MOUSE_RECORDER_NO_SAMPLE;
  }

  recorder->lastPollTimeMs = currentTimeMs;
  recorder->hasPolled = TRUE;

  POINT currentPosition;

  if (!GetCursorPos(&currentPosition)) {
    return MOUSE_RECORDER_ERROR;
  }

  if (recorder->hasLastPosition &&
      !mouseRecorderPositionChanged(recorder->lastPosition, currentPosition)) {
    return MOUSE_RECORDER_NO_SAMPLE;
  }

  if (recorder->sampleCount == SIZE_MAX ||
      !mouseRecorderReserve(recorder, recorder->sampleCount + 1)) {
    return MOUSE_RECORDER_ERROR;
  }

  MouseRecorderSample *sample = &recorder->samples[recorder->sampleCount];
  sample->position = currentPosition;
  sample->timestampMs = currentTimeMs - recorder->startTimeMs;

  recorder->sampleCount++;
  recorder->lastPosition = currentPosition;
  recorder->hasLastPosition = TRUE;

  return MOUSE_RECORDER_SAMPLE_RECORDED;
}

void mouseRecorderStop(MouseRecorder *recorder) {
  if (recorder != NULL) {
    recorder->isRecording = FALSE;
  }
}

void mouseRecorderClear(MouseRecorder *recorder) {
  if (recorder == NULL) {
    return;
  }

  recorder->sampleCount = 0;
  recorder->hasLastPosition = FALSE;
  recorder->hasPolled = FALSE;
}

void mouseRecorderDestroy(MouseRecorder *recorder) {
  if (recorder == NULL) {
    return;
  }

  free(recorder->samples);
  ZeroMemory(recorder, sizeof(*recorder));
}

const MouseRecorderSample *
mouseRecorderGetSamples(const MouseRecorder *recorder, size_t *sampleCount) {
  if (sampleCount != NULL) {
    *sampleCount = recorder != NULL ? recorder->sampleCount : 0;
  }

  if (recorder == NULL) {
    return NULL;
  }

  return recorder->samples;
}