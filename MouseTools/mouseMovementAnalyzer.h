#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

typedef enum {
  MOUSE_DIRECTION_STATIONARY = 0,
  MOUSE_DIRECTION_RIGHT,
  MOUSE_DIRECTION_DOWN_RIGHT,
  MOUSE_DIRECTION_DOWN,
  MOUSE_DIRECTION_DOWN_LEFT,
  MOUSE_DIRECTION_LEFT,
  MOUSE_DIRECTION_UP_LEFT,
  MOUSE_DIRECTION_UP,
  MOUSE_DIRECTION_UP_RIGHT
} MouseMovementDirection;

typedef struct {
  POINT position;
  double distancePixels;
  double speedPixelsPerSecond;
  double directionDegrees;
  MouseMovementDirection direction;
  ULONGLONG elapsedMs;
  ULONGLONG timestampMs;
  BOOL hasPreviousSample;
} MouseMovementData;

typedef struct {
  POINT previousPosition;
  ULONGLONG previousTimeMs;
  BOOL initialized;
} MouseMovementAnalyzer;

void mouseMovementAnalyzerInitialize(MouseMovementAnalyzer *analyzer);
BOOL mouseMovementAnalyzerUpdate(MouseMovementAnalyzer *analyzer,
                                 MouseMovementData *data);
void mouseMovementAnalyzerReset(MouseMovementAnalyzer *analyzer);
