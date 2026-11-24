#include "colourChangeDetector.h"

#include <math.h>

BOOL colourChangeDetectorInitialize(PixelColourChangeDetector *detector, int x,
                                    int y, DWORD intervalMs, double threshold) {
  if (detector == NULL || intervalMs == 0 || intervalMs > 60000u ||
      !isfinite(threshold) || threshold < 0.0)
    return FALSE;

  PixelColor baseline;
  if (!pixelReadColorAt(x, y, &baseline))
    return FALSE;

  ZeroMemory(detector, sizeof(*detector));
  detector->x = x;
  detector->y = y;
  detector->intervalMs = intervalMs;
  detector->threshold = threshold;
  detector->previousColor = baseline;
  detector->lastSampleTime = GetTickCount64();
  detector->initialized = TRUE;
  return TRUE;
}

BOOL colourChangeDetectorUpdate(PixelColourChangeDetector *detector,
                                BOOL *changed, PixelColor *previous,
                                PixelColor *current, double *difference,
                                ULONGLONG *timestampMs) {
  if (detector == NULL || !detector->initialized)
    return FALSE;

  ULONGLONG now = GetTickCount64();
  if (now - detector->lastSampleTime < detector->intervalMs)
    return FALSE;

  PixelColor sampled;
  if (!pixelReadColorAt(detector->x, detector->y, &sampled))
    return FALSE;

  PixelColor old = detector->previousColor;
  double delta = pixelColorDistance(old, sampled);
  detector->previousColor = sampled;
  detector->lastSampleTime = now;

  if (previous != NULL)
    *previous = old;
  if (current != NULL)
    *current = sampled;
  if (difference != NULL)
    *difference = delta;
  if (timestampMs != NULL)
    *timestampMs = now;
  if (changed != NULL)
    *changed = delta >= detector->threshold;

  return TRUE;
}