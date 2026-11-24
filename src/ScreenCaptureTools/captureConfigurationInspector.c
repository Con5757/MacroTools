#define WIN32_LEAN_AND_MEAN
#include "captureConfigurationInspector.h"
#include <string.h>

typedef struct {
  CaptureConfiguration *configuration;
} MonitorEnumContext;

static BOOL CALLBACK inspectMonitorCallback(HMONITOR monitor, HDC dc,
                                            LPRECT rect, LPARAM parameter) {
  (void)dc;
  (void)rect;
  MonitorEnumContext *context = (MonitorEnumContext *)parameter;
  CaptureConfiguration *configuration = context->configuration;
  if (configuration->monitorCount >= CAPTURE_CONFIGURATION_MAX_MONITORS)
    return FALSE;

  MONITORINFOEXA info;
  ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);
  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info))
    return TRUE;

  CaptureMonitorConfiguration *entry =
      &configuration->monitors[configuration->monitorCount++];
  memcpy(entry->deviceName, info.szDevice, sizeof(entry->deviceName));
  entry->deviceName[CCHDEVICENAME - 1] = '\0';
  entry->bounds = info.rcMonitor;
  entry->workArea = info.rcWork;
  entry->width = (UINT)(info.rcMonitor.right - info.rcMonitor.left);
  entry->height = (UINT)(info.rcMonitor.bottom - info.rcMonitor.top);
  entry->primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
  return TRUE;
}

BOOL captureConfigurationInspect(CaptureConfiguration *configuration) {
  if (configuration == NULL)
    return FALSE;
  ZeroMemory(configuration, sizeof(*configuration));
  LONG left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  LONG top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  LONG width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  LONG height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  if (width <= 0 || height <= 0)
    return FALSE;
  configuration->virtualDesktopBounds.left = left;
  configuration->virtualDesktopBounds.top = top;
  configuration->virtualDesktopBounds.right = left + width;
  configuration->virtualDesktopBounds.bottom = top + height;
  configuration->virtualDesktopWidth = (UINT)width;
  configuration->virtualDesktopHeight = (UINT)height;

  MonitorEnumContext context = {configuration};
  if (!EnumDisplayMonitors(NULL, NULL, inspectMonitorCallback,
                           (LPARAM)&context))
    return FALSE;
  return configuration->monitorCount > 0;
}

BOOL captureConfigurationInspectImage(
    const CaptureImage *image, CaptureImageConfiguration *configuration) {
  if (image == NULL || configuration == NULL || image->pixels == NULL)
    return FALSE;
  configuration->width = image->width;
  configuration->height = image->height;
  configuration->stride = image->stride;
  configuration->dataSize = image->dataSize;
  configuration->pixelFormat = "32-bit BGRA (top-down)";
  configuration->timestampMs = image->timestampMs;
  return TRUE;
}