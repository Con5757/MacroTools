#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

typedef struct {
  POINT previousPosition;
  double totalDistancePixels;
  double lastSegmentDistancePixels;
  ULONGLONG sampleCount;
  BOOL initialized;
} MouseDistanceTracker;

void mouseDistanceTrackerInitialize(MouseDistanceTracker *tracker);
BOOL mouseDistanceTrackerUpdate(MouseDistanceTracker *tracker,
                                double *segmentDistancePixels);
void mouseDistanceTrackerReset(MouseDistanceTracker *tracker);
double mouseDistanceTrackerGetTotal(const MouseDistanceTracker *tracker);