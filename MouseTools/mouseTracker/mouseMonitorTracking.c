#include "mouseMonitorTracking.h"

BOOL mouseMonitorTracking(const POINT *position, MouseMonitorData *data) {
  if (position == NULL || data == NULL) {
    return FALSE;
  }

  ZeroMemory(data, sizeof(*data));

  HMONITOR monitor = MonitorFromPoint(*position, MONITOR_DEFAULTTONULL);
  if (monitor == NULL) {
    return FALSE;
  }

  MONITORINFOEXA info;
  ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
    return FALSE;
  }

  data->handle = monitor;
  data->bounds = info.rcMonitor;
  data->width = info.rcMonitor.right - info.rcMonitor.left;
  data->height = info.rcMonitor.bottom - info.rcMonitor.top;
  data->isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;

  CopyMemory(data->name, info.szDevice, sizeof(data->name));
  data->name[CCHDEVICENAME - 1] = '\0';

  return TRUE;
}
