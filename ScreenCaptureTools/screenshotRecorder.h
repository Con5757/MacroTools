#pragma once
#include "common/captureWic.h"

#define SCREENSHOT_RECORDER_PATH_CAPACITY 520
#define SCREENSHOT_RECORDER_PREFIX_CAPACITY 64

typedef struct {
  char directory[SCREENSHOT_RECORDER_PATH_CAPACITY];
  char prefix[SCREENSHOT_RECORDER_PREFIX_CAPACITY];
  DWORD intervalMs;
  ULONGLONG lastCaptureMs;
  unsigned long sequence;
  CaptureImageFormat format;
  BOOL active;
  BOOL hasCaptured;
} ScreenshotRecorder;

BOOL screenshotRecorderStart(ScreenshotRecorder *recorder,
                             const char *directoryUtf8,
                             const char *filenamePrefix,
                             CaptureImageFormat format, DWORD intervalMs);
void screenshotRecorderStop(ScreenshotRecorder *recorder);
BOOL screenshotRecorderUpdate(ScreenshotRecorder *recorder, char *savedPath,
                              size_t savedPathCapacity);