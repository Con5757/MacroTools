#define WIN32_LEAN_AND_MEAN
#include "monitorSwitchDetector.h"
#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define SLEEP_DURATION 100 // can be changed later, modifiable in ms

void monitorSwitchDetector(void) {
  POINT point;
  HMONITOR monitor;
  MONITORINFOEXA info;
  char previousMonitor[CCHDEVICENAME] = "";
  BOOL initialized = FALSE;

  printf("\nMonitor Switch Detector\n");
  printf("Move your cursor between monitors. Press Ctrl+C to stop.\n");

  while (TRUE) {
    if (!GetCursorPos(&point)) {
      Sleep(10);
      continue;
    }

    monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONULL);

    if (monitor != NULL) {
      ZeroMemory(&info, sizeof(info));
      info.cbSize = sizeof(info);

      if (GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
        if (!initialized) {
          strncpy(previousMonitor, info.szDevice, sizeof(previousMonitor) - 1);
          initialized = TRUE;

          printf("Current monitor: %s\n", info.szDevice);
        } else if (strcmp(previousMonitor, info.szDevice) != 0) {
          printf("Monitor switch: %s -> %s (X=%ld Y=%ld)\n", previousMonitor,
                 info.szDevice, point.x, point.y);

          strncpy(previousMonitor, info.szDevice, sizeof(previousMonitor) - 1);
        }
      }
    }

    Sleep(SLEEP_DURATION); 
  }
}