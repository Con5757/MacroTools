#define WIN32_LEAN_AND_MEAN
#include "displayScalingInspector.h"
#include <ShellScalingApi.h>
#include <Windows.h>
#include <stdio.h>

static BOOL CALLBACK displayScalingCallback(HMONITOR monitor, HDC dc,
                                            LPRECT rect, LPARAM data) {
  MONITORINFOEXA info;
  UINT dpiX = 0;
  UINT dpiY = 0;

  ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
    return TRUE;
  }

  printf("%s: ", info.szDevice);

  HRESULT result = GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

  if (SUCCEEDED(result)) {
    printf("DPI: %u x %u | Scaling: %.0f%%\n", dpiX, dpiY,
           (dpiX / 96.0) * 100.0);
  } else {
    printf("Scaling unavailable. HRESULT: 0x%08lX\n", (unsigned long)result);
  }

  return TRUE;
}

void displayScalingInspector(void) {
  printf("\nDisplay Scaling Inspector\n");

  if (!EnumDisplayMonitors(NULL, NULL, displayScalingCallback, 0)) {
    printf("Display scaling inspection failed.\n");
  }
}