#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "mouseTracker/mouseMonitorTracking.h"
#include <Windows.h>

typedef struct {
  DWORD intervalMs;
  ULONGLONG lastSampleTime;
  BOOL hasSample;
} MouseTracker;

typedef struct {
  POINT desktopPosition;
  POINT monitorPosition;
  MouseMonitorData monitor;
  BOOL monitorFound;
} MouseTrackerData;

BOOL mouseTrackerInitialize(MouseTracker *tracker, DWORD intervalMs);
BOOL mouseTrackerUpdate(MouseTracker *tracker, MouseTrackerData *data);
void mouseTracker(void);
