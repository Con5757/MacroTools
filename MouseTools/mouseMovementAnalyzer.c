#define WIN32_LEAN_AND_MEAN
#include "mouseMovementAnalyzer.h"

#include <math.h>

#define MOUSE_MOVEMENT_PI 3.14159265358979323846

static MouseMovementDirection mouseMovementClassify(double angleDegrees) {
  if (angleDegrees >= -22.5 && angleDegrees < 22.5)
    return MOUSE_DIRECTION_RIGHT;
  if (angleDegrees >= 22.5 && angleDegrees < 67.5)
    return MOUSE_DIRECTION_DOWN_RIGHT;
  if (angleDegrees >= 67.5 && angleDegrees < 112.5)
    return MOUSE_DIRECTION_DOWN;
  if (angleDegrees >= 112.5 && angleDegrees < 157.5)
    return MOUSE_DIRECTION_DOWN_LEFT;
  if (angleDegrees >= 157.5 || angleDegrees < -157.5)
    return MOUSE_DIRECTION_LEFT;
  if (angleDegrees >= -157.5 && angleDegrees < -112.5)
    return MOUSE_DIRECTION_UP_LEFT;
  if (angleDegrees >= -112.5 && angleDegrees < -67.5)
    return MOUSE_DIRECTION_UP;
  return MOUSE_DIRECTION_UP_RIGHT;
}

void mouseMovementAnalyzerInitialize(MouseMovementAnalyzer *analyzer) {
  if (analyzer != NULL) {
    ZeroMemory(analyzer, sizeof(*analyzer));
  }
}

BOOL mouseMovementAnalyzerUpdate(MouseMovementAnalyzer *analyzer,
                                 MouseMovementData *data) {
  if (analyzer == NULL || data == NULL) {
    return FALSE;
  }

  POINT current;
  if (!GetCursorPos(&current)) {
    return FALSE;
  }

  ULONGLONG now = GetTickCount64();
  ZeroMemory(data, sizeof(*data));
  data->position = current;
  data->timestampMs = now;
  data->direction = MOUSE_DIRECTION_STATIONARY;

  if (!analyzer->initialized) {
    analyzer->previousPosition = current;
    analyzer->previousTimeMs = now;
    analyzer->initialized = TRUE;
    return TRUE;
  }

  LONG dx = current.x - analyzer->previousPosition.x;
  LONG dy = current.y - analyzer->previousPosition.y;
  ULONGLONG elapsed = now - analyzer->previousTimeMs;
  double distance = sqrt((double)dx * dx + (double)dy * dy);

  data->distancePixels = distance;
  data->elapsedMs = elapsed;
  data->hasPreviousSample = TRUE;

  if (elapsed > 0) {
    data->speedPixelsPerSecond = distance * 1000.0 / (double)elapsed;
  }

  if (dx != 0 || dy != 0) {
    data->directionDegrees =
        atan2((double)dy, (double)dx) * 180.0 / MOUSE_MOVEMENT_PI;
    data->direction = mouseMovementClassify(data->directionDegrees);
  }

  analyzer->previousPosition = current;
  analyzer->previousTimeMs = now;
  return TRUE;
}

void mouseMovementAnalyzerReset(MouseMovementAnalyzer *analyzer) {
  mouseMovementAnalyzerInitialize(analyzer);
}