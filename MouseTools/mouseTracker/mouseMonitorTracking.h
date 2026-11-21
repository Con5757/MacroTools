#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

typedef struct {
  HMONITOR handle;
  RECT bounds;
  char name[CCHDEVICENAME];
  LONG width;
  LONG height;
  BOOL isPrimary;
} MouseMonitorData;

BOOL mouseMonitorTracking(const POINT *position, MouseMonitorData *data);
