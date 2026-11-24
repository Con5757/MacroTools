#define WIN32_LEAN_AND_MEAN
#include "monitorCapture.h"

BOOL monitorCapture(HMONITOR monitor, CaptureImage *image) {
  if (monitor == NULL || image == NULL)
    return FALSE;
  MONITORINFO info;
  ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);
  if (!GetMonitorInfoA(monitor, &info))
    return FALSE;
  return captureImageCaptureRect(&info.rcMonitor, image);
}