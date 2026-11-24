#define WIN32_LEAN_AND_MEAN
#include "monitorRes.h"
#include <Windows.h>
#include <stdio.h>

static BOOL CALLBACK monitorResCallback(HMONITOR monitor, HDC dc, LPRECT rect,
                                        LPARAM data) {
  MONITORINFOEXA info;
  DEVMODEA mode;

  ZeroMemory(&info, sizeof(info));
  ZeroMemory(&mode, sizeof(mode));

  info.cbSize = sizeof(info);
  mode.dmSize = sizeof(mode);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info))
    return TRUE;

  printf("%s: ", info.szDevice);

  if (EnumDisplaySettingsA(info.szDevice, ENUM_CURRENT_SETTINGS, &mode))
    printf("%lu x %lu @ %lu Hz\n", mode.dmPelsWidth, mode.dmPelsHeight,
           mode.dmDisplayFrequency);
  else
    printf("resolution unavailable\n");

  return TRUE;
}

void monitorRes(void) {
  printf("\nMonitor Resolution\n");
  if (!EnumDisplayMonitors(NULL, NULL, monitorResCallback, 0))
    printf("attempt to get resolution failed\n");
}