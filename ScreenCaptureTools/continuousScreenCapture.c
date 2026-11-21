#define WIN32_LEAN_AND_MEAN
#include "continuousScreenCapture.h"
#include "screenCapture.h"

BOOL continuousScreenCaptureInitialize(ContinuousScreenCapture *capture,
                                       DWORD intervalMs,
                                       ContinuousCaptureSource source,
                                       HMONITOR monitor, const RECT *region,
                                       HWND window) {
  if (capture == NULL || intervalMs == 0 || intervalMs > 3600000u ||
      source < CONTINUOUS_CAPTURE_DESKTOP || source > CONTINUOUS_CAPTURE_WINDOW)
    return FALSE;
  if (source == CONTINUOUS_CAPTURE_MONITOR && monitor == NULL)
    return FALSE;
  if (source == CONTINUOUS_CAPTURE_REGION &&
      (region == NULL || region->right <= region->left ||
       region->bottom <= region->top))
    return FALSE;
  if (source == CONTINUOUS_CAPTURE_WINDOW &&
      (window == NULL || !IsWindow(window)))
    return FALSE;

  ZeroMemory(capture, sizeof(*capture));
  capture->intervalMs = intervalMs;
  capture->source = source;
  capture->monitor = monitor;
  capture->window = window;
  if (region != NULL)
    capture->region = *region;
  capture->active = TRUE;
  return TRUE;
}

void continuousScreenCaptureStop(ContinuousScreenCapture *capture) {
  if (capture != NULL)
    capture->active = FALSE;
}

BOOL continuousScreenCaptureUpdate(ContinuousScreenCapture *capture,
                                   CaptureImage *image) {
  if (capture == NULL || image == NULL || !capture->active)
    return FALSE;
  ULONGLONG now = GetTickCount64();
  if (capture->hasCaptured &&
      now - capture->lastCaptureMs < capture->intervalMs)
    return FALSE;

  BOOL success = FALSE;
  switch (capture->source) {
  case CONTINUOUS_CAPTURE_DESKTOP:
    success = screenCaptureFullDesktop(image);
    break;
  case CONTINUOUS_CAPTURE_MONITOR:
    success = screenCaptureMonitor(capture->monitor, image);
    break;
  case CONTINUOUS_CAPTURE_REGION:
    success = screenCaptureRegion(&capture->region, image);
    break;
  case CONTINUOUS_CAPTURE_WINDOW:
    success = screenCaptureWindow(capture->window, image);
    break;
  default:
    return FALSE;
  }
  if (success) {
    capture->lastCaptureMs = GetTickCount64();
    capture->hasCaptured = TRUE;
  }
  return success;
}