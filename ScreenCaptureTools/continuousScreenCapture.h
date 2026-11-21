#pragma once
#include "common/captureImage.h"

typedef enum {
  CONTINUOUS_CAPTURE_DESKTOP = 0,
  CONTINUOUS_CAPTURE_MONITOR,
  CONTINUOUS_CAPTURE_REGION,
  CONTINUOUS_CAPTURE_WINDOW
} ContinuousCaptureSource;

typedef struct {
  DWORD intervalMs;
  ULONGLONG lastCaptureMs;
  ContinuousCaptureSource source;
  HMONITOR monitor;
  RECT region;
  HWND window;
  BOOL active;
  BOOL hasCaptured;
} ContinuousScreenCapture;

BOOL continuousScreenCaptureInitialize(ContinuousScreenCapture *capture,
                                       DWORD intervalMs,
                                       ContinuousCaptureSource source,
                                       HMONITOR monitor, const RECT *region,
                                       HWND window);
void continuousScreenCaptureStop(ContinuousScreenCapture *capture);
BOOL continuousScreenCaptureUpdate(ContinuousScreenCapture *capture,
                                   CaptureImage *image);