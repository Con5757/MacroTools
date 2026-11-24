#pragma once

#include "pixelToolsCommon.h"

typedef struct {
  ULONGLONG elapsedMs;
  PixelColor color;
} PixelHistorySample;

typedef struct {
  int x;
  int y;
  DWORD intervalMs;
  ULONGLONG startTime;
  ULONGLONG lastSampleTime;
  PixelHistorySample *samples;
  size_t count;
  size_t capacity;
  BOOL active;
} PixelHistoryRecorder;

BOOL pixelHistoryRecorderInitialize(PixelHistoryRecorder *recorder, int x,
                                    int y, DWORD intervalMs);
BOOL pixelHistoryRecorderStart(PixelHistoryRecorder *recorder,
                               BOOL clearExisting);
BOOL pixelHistoryRecorderUpdate(PixelHistoryRecorder *recorder);
void pixelHistoryRecorderStop(PixelHistoryRecorder *recorder);
void pixelHistoryRecorderClear(PixelHistoryRecorder *recorder);
void pixelHistoryRecorderDestroy(PixelHistoryRecorder *recorder);
const PixelHistorySample *
pixelHistoryRecorderGetSamples(const PixelHistoryRecorder *recorder,
                               size_t *count);
