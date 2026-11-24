#pragma once
#include "common/captureImage.h"

#define CAPTURE_CONFIGURATION_MAX_MONITORS 32

typedef struct {
  char deviceName[CCHDEVICENAME];
  RECT bounds;
  RECT workArea;
  UINT width;
  UINT height;
  BOOL primary;
} CaptureMonitorConfiguration;

typedef struct {
  RECT virtualDesktopBounds;
  UINT virtualDesktopWidth;
  UINT virtualDesktopHeight;
  int monitorCount;
  CaptureMonitorConfiguration monitors[CAPTURE_CONFIGURATION_MAX_MONITORS];
} CaptureConfiguration;

typedef struct {
  UINT width;
  UINT height;
  UINT stride;
  size_t dataSize;
  const char *pixelFormat;
  ULONGLONG timestampMs;
} CaptureImageConfiguration;

BOOL captureConfigurationInspect(CaptureConfiguration *configuration);
BOOL captureConfigurationInspectImage(const CaptureImage *image,
                                      CaptureImageConfiguration *configuration);