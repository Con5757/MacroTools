#define WIN32_LEAN_AND_MEAN
#include "resolutionDetector.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

static BOOL CALLBACK resolutionCallback(HMONITOR monitor, HDC dc, LPRECT rect,
                                        LPARAM data) {
  MONITORINFOEXA info;
  DEVMODEA mode;

  ZeroMemory(&info, sizeof(info));
  ZeroMemory(&mode, sizeof(mode));

  info.cbSize = sizeof(info);
  mode.dmSize = sizeof(mode);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
    return TRUE;
  }

  if (EnumDisplaySettingsA(info.szDevice, ENUM_CURRENT_SETTINGS, &mode)) {
    printf("%s: %lu x %lu @ %lu Hz\n", info.szDevice, mode.dmPelsWidth,
           mode.dmPelsHeight, mode.dmDisplayFrequency);
  }

  return TRUE;
}

void resolutionDetector(void) {
  printf("\nResolution Detector\n");
  printf("Monitoring display resolutions. Press Ctrl+C to stop.\n");

  while (TRUE) {
    system("cls");
    printf("Resolution Detector\n\n");

    if (!EnumDisplayMonitors(NULL, NULL, resolutionCallback, 0)) {
      printf("Resolution detection failed.\n");
    }

    Sleep(1000);
  }
}