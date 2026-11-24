#include "pixelHistoryRecorder.h"

#include <stdlib.h>

BOOL pixelHistoryRecorderInitialize(PixelHistoryRecorder *recorder, int x,
                                    int y, DWORD intervalMs) {
  if (recorder == NULL || intervalMs == 0 || intervalMs > 60000u)
    return FALSE;
  ZeroMemory(recorder, sizeof(*recorder));
  recorder->x = x;
  recorder->y = y;
  recorder->intervalMs = intervalMs;
  return TRUE;
}

BOOL pixelHistoryRecorderStart(PixelHistoryRecorder *recorder,
                               BOOL clearExisting) {
  if (recorder == NULL)
    return FALSE;
  if (clearExisting)
    pixelHistoryRecorderClear(recorder);
  ULONGLONG now = GetTickCount64();
  if (recorder->count == 0) {
    recorder->startTime = now;
  } else {
    ULONGLONG lastElapsed = recorder->samples[recorder->count - 1u].elapsedMs;
    recorder->startTime = now >= lastElapsed ? now - lastElapsed : 0;
  }
  recorder->lastSampleTime = 0;
  recorder->active = TRUE;
  return TRUE;
}

BOOL pixelHistoryRecorderUpdate(PixelHistoryRecorder *recorder) {
  if (recorder == NULL || !recorder->active)
    return FALSE;

  ULONGLONG now = GetTickCount64();
  if (recorder->lastSampleTime != 0 &&
      now - recorder->lastSampleTime < recorder->intervalMs)
    return FALSE;

  PixelColor color;
  if (!pixelReadColorAt(recorder->x, recorder->y, &color))
    return FALSE;

  if (recorder->count == recorder->capacity) {
    size_t newCapacity =
        recorder->capacity == 0 ? 128u : recorder->capacity * 2u;
    if (newCapacity < recorder->capacity ||
        newCapacity > SIZE_MAX / sizeof(PixelHistorySample))
      return FALSE;
    PixelHistorySample *newSamples = (PixelHistorySample *)realloc(
        recorder->samples, newCapacity * sizeof(PixelHistorySample));
    if (newSamples == NULL)
      return FALSE;
    recorder->samples = newSamples;
    recorder->capacity = newCapacity;
  }

  recorder->samples[recorder->count].elapsedMs = now - recorder->startTime;
  recorder->samples[recorder->count].color = color;
  ++recorder->count;
  recorder->lastSampleTime = now;
  return TRUE;
}

void pixelHistoryRecorderStop(PixelHistoryRecorder *recorder) {
  if (recorder != NULL)
    recorder->active = FALSE;
}

void pixelHistoryRecorderClear(PixelHistoryRecorder *recorder) {
  if (recorder == NULL)
    return;
  recorder->count = 0;
  recorder->lastSampleTime = 0;
  if (recorder->active)
    recorder->startTime = GetTickCount64();
}

void pixelHistoryRecorderDestroy(PixelHistoryRecorder *recorder) {
  if (recorder == NULL)
    return;
  free(recorder->samples);
  ZeroMemory(recorder, sizeof(*recorder));
}

const PixelHistorySample *
pixelHistoryRecorderGetSamples(const PixelHistoryRecorder *recorder,
                               size_t *count) {
  if (count != NULL)
    *count = recorder != NULL ? recorder->count : 0;
  return recorder != NULL ? recorder->samples : NULL;
}