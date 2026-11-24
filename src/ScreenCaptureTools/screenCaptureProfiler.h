#pragma once
#include "common/captureImage.h"

typedef BOOL (*ScreenCaptureOperation)(void *context, CaptureImage *image);

typedef struct {
  unsigned long long sampleCount;
  double totalMilliseconds;
  double minimumMilliseconds;
  double maximumMilliseconds;
} ScreenCaptureProfiler;

typedef struct {
  unsigned long long sampleCount;
  double averageMilliseconds;
  double minimumMilliseconds;
  double maximumMilliseconds;
  double totalMilliseconds;
} ScreenCaptureProfileResult;

void screenCaptureProfilerInitialize(ScreenCaptureProfiler *profiler);
void screenCaptureProfilerReset(ScreenCaptureProfiler *profiler);
void screenCaptureProfilerRecord(ScreenCaptureProfiler *profiler,
                                 double elapsedMilliseconds);
BOOL screenCaptureProfilerRunSample(ScreenCaptureProfiler *profiler,
                                    ScreenCaptureOperation operation,
                                    void *context);
BOOL screenCaptureProfilerGetResult(const ScreenCaptureProfiler *profiler,
                                    ScreenCaptureProfileResult *result);