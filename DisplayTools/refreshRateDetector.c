#define WIN32_LEAN_AND_MEAN
#include "refreshRateDetector.h"
#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_MONITORS 32

typedef struct {
  char deviceName[CCHDEVICENAME];
  DWORD refreshRate;
  DWORD width;
  DWORD height;
} RefreshRateInfo;

typedef struct {
  RefreshRateInfo monitors[MAX_MONITORS];
  int count;
} RefreshRateSnapshot;

static BOOL CALLBACK refreshRateCallback(HMONITOR monitor, HDC dc, LPRECT rect,
                                         LPARAM data) {
  RefreshRateSnapshot *snapshot = (RefreshRateSnapshot *)data;

  if (snapshot->count >= MAX_MONITORS) {
    return TRUE;
  }

  MONITORINFOEXA info;
  DEVMODEA mode;

  ZeroMemory(&info, sizeof(info));
  ZeroMemory(&mode, sizeof(mode));

  info.cbSize = sizeof(info);
  mode.dmSize = sizeof(mode);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
    return TRUE;
  }

  if (!EnumDisplaySettingsA(info.szDevice, ENUM_CURRENT_SETTINGS, &mode)) {
    return TRUE;
  }

  RefreshRateInfo *result = &snapshot->monitors[snapshot->count];

  strncpy(result->deviceName, info.szDevice, CCHDEVICENAME - 1);
  result->refreshRate = mode.dmDisplayFrequency;
  result->width = mode.dmPelsWidth;
  result->height = mode.dmPelsHeight;

  snapshot->count++;

  return TRUE;
}

static int findMonitor(RefreshRateSnapshot *snapshot, const char *deviceName) {
  for (int i = 0; i < snapshot->count; i++) {
    if (strcmp(snapshot->monitors[i].deviceName, deviceName) == 0) {
      return i;
    }
  }

  return -1;
}

static void printRefreshRate(DWORD refreshRate) {
  if (refreshRate <= 1) {
    printf("Default");
  } else {
    printf("%lu Hz", refreshRate);
  }
}

void refreshRateDetector(void) {
  RefreshRateSnapshot previous = {0};
  BOOL initialized = FALSE;

  printf("\nRefresh Rate Detector\n");
  printf("Monitoring refresh rates. Press Ctrl+C to stop.\n");

  while (TRUE) {
    RefreshRateSnapshot current = {0};

    if (!EnumDisplayMonitors(NULL, NULL, refreshRateCallback,
                             (LPARAM)&current)) {
      printf("Refresh rate detection failed.\n");
      Sleep(1000);
      continue;
    }

    if (!initialized) {
      for (int i = 0; i < current.count; i++) {
        printf("%s: ", current.monitors[i].deviceName);
        printRefreshRate(current.monitors[i].refreshRate);

        printf(" | %lu x %lu\n", current.monitors[i].width,
               current.monitors[i].height);
      }

      initialized = TRUE;
    } else {
      for (int i = 0; i < current.count; i++) {
        RefreshRateInfo *monitor = &current.monitors[i];

        int previousIndex = findMonitor(&previous, monitor->deviceName);

        if (previousIndex == -1) {
          printf("\nMonitor detected: %s\n", monitor->deviceName);
          printf("Refresh rate: ");
          printRefreshRate(monitor->refreshRate);
          printf("\n");
        } else if (previous.monitors[previousIndex].refreshRate !=
                   monitor->refreshRate) {
          printf("\nRefresh rate changed: %s\n", monitor->deviceName);
          printf("Previous: ");
          printRefreshRate(previous.monitors[previousIndex].refreshRate);
          printf("\nCurrent: ");
          printRefreshRate(monitor->refreshRate);
          printf("\n");
        }
      }

      for (int i = 0; i < previous.count; i++) {
        if (findMonitor(&current, previous.monitors[i].deviceName) == -1) {
          printf("\nMonitor disconnected: %s\n",
                 previous.monitors[i].deviceName);
        }
      }
    }

    previous = current;

    Sleep(500);
  }
}