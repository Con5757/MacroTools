#define WIN32_LEAN_AND_MEAN
#include "coordConverter.h"
#include <Windows.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MONITORS 32

typedef struct {
  HMONITOR handle;
  MONITORINFOEXA info;
  LONG width;
  LONG height;
} MonitorData;

typedef struct {
  MonitorData monitors[MAX_MONITORS];
  int count;
} MonitorList;

static BOOL CALLBACK coordConverterCallback(HMONITOR monitor, HDC dc,
                                            LPRECT rect, LPARAM data) {
  MonitorList *list = (MonitorList *)data;

  if (list->count >= MAX_MONITORS) {
    return TRUE;
  }

  MonitorData *entry = &list->monitors[list->count];

  ZeroMemory(entry, sizeof(*entry));
  entry->info.cbSize = sizeof(entry->info);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&entry->info)) {
    return TRUE;
  }

  entry->handle = monitor;
  entry->width = entry->info.rcMonitor.right - entry->info.rcMonitor.left;
  entry->height = entry->info.rcMonitor.bottom - entry->info.rcMonitor.top;

  list->count++;
  return TRUE;
}

static int readLong(const char *prompt, LONG *value) {
  char input[128];
  char *end;

  printf("%s", prompt);

  if (!fgets(input, sizeof(input), stdin)) {
    return 0;
  }

  long result = strtol(input, &end, 10);

  if (end == input || result < LONG_MIN || result > LONG_MAX) {
    printf("Invalid input.\n");
    return 0;
  }

  *value = (LONG)result;
  return 1;
}

static void printMonitors(const MonitorList *list) {
  for (int i = 0; i < list->count; i++) {
    const MonitorData *monitor = &list->monitors[i];

    printf("%d. %s | %ldx%ld | Position: %ld,%ld%s\n", i + 1,
           monitor->info.szDevice, monitor->width, monitor->height,
           monitor->info.rcMonitor.left, monitor->info.rcMonitor.top,
           (monitor->info.dwFlags & MONITORINFOF_PRIMARY) ? " | Primary" : "");
  }
}

static int selectMonitor(const MonitorList *list, const char *prompt) {
  LONG selection;

  if (!readLong(prompt, &selection)) {
    return -1;
  }

  if (selection < 1 || selection > list->count) {
    printf("Invalid monitor selection.\n");
    return -1;
  }

  return (int)selection - 1;
}

static int convertDesktopToMonitor(const MonitorList *list) {
  LONG x;
  LONG y;

  if (!readLong("Desktop X: ", &x) || !readLong("Desktop Y: ", &y)) {
    return 0;
  }

  POINT point = {x, y};
  int found = -1;

  for (int i = 0; i < list->count; i++) {
    if (PtInRect(&list->monitors[i].info.rcMonitor, point)) {
      found = i;
      break;
    }
  }

  if (found == -1) {
    printf("Coordinates are outside all connected monitors.\n");
    return 0;
  }

  const MonitorData *monitor = &list->monitors[found];

  LONG relativeX = x - monitor->info.rcMonitor.left;
  LONG relativeY = y - monitor->info.rcMonitor.top;

  printf("\nMonitor: %s\n", monitor->info.szDevice);
  printf("Monitor-relative X: %ld\n", relativeX);
  printf("Monitor-relative Y: %ld\n", relativeY);
  printf("Resolution: %ldx%ld\n", monitor->width, monitor->height);

  printf("Normalized position: %.2f%%, %.2f%%\n",
         (double)relativeX / (monitor->width - 1) * 100.0,
         (double)relativeY / (monitor->height - 1) * 100.0);

  return 1;
}

static int convertMonitorToDesktop(const MonitorList *list) {
  printMonitors(list);

  int selection = selectMonitor(list, "Select monitor: ");

  if (selection == -1) {
    return 0;
  }

  const MonitorData *monitor = &list->monitors[selection];

  LONG x;
  LONG y;

  if (!readLong("Monitor-relative X: ", &x) ||
      !readLong("Monitor-relative Y: ", &y)) {
    return 0;
  }

  if (x < 0 || x >= monitor->width || y < 0 || y >= monitor->height) {
    printf("Coordinates are outside the selected monitor.\n");
    return 0;
  }

  printf("\nDesktop X: %ld\n", monitor->info.rcMonitor.left + x);
  printf("Desktop Y: %ld\n", monitor->info.rcMonitor.top + y);

  return 1;
}

static int convertBetweenMonitors(const MonitorList *list) {
  printMonitors(list);

  int sourceSelection = selectMonitor(list, "Source monitor: ");

  if (sourceSelection == -1) {
    return 0;
  }

  int targetSelection = selectMonitor(list, "Target monitor: ");

  if (targetSelection == -1) {
    return 0;
  }

  const MonitorData *source = &list->monitors[sourceSelection];
  const MonitorData *target = &list->monitors[targetSelection];

  LONG x;
  LONG y;

  if (!readLong("Source X: ", &x) || !readLong("Source Y: ", &y)) {
    return 0;
  }

  if (x < 0 || x >= source->width || y < 0 || y >= source->height) {
    printf("Coordinates are outside the source monitor.\n");
    return 0;
  }

  double normalizedX = (double)x / (source->width - 1);
  double normalizedY = (double)y / (source->height - 1);

  LONG targetX = (LONG)(normalizedX * (target->width - 1) + 0.5);
  LONG targetY = (LONG)(normalizedY * (target->height - 1) + 0.5);

  printf("\nTarget monitor: %s\n", target->info.szDevice);
  printf("Target-relative X: %ld\n", targetX);
  printf("Target-relative Y: %ld\n", targetY);
  printf("Target desktop X: %ld\n", target->info.rcMonitor.left + targetX);
  printf("Target desktop Y: %ld\n", target->info.rcMonitor.top + targetY);

  return 1;
}

void coordConverter(void) {
  MonitorList list = {0};

  printf("\nCoordinate Converter\n\n");

  if (!EnumDisplayMonitors(NULL, NULL, coordConverterCallback, (LPARAM)&list)) {
    printf("Monitor enumeration failed.\n");
    return;
  }

  if (list.count == 0) {
    printf("No monitors detected.\n");
    return;
  }

  printf("Connected Monitors: %d\n\n", list.count);

  printf("1. Desktop to Monitor Coordinates\n");
  printf("2. Monitor to Desktop Coordinates\n");
  printf("3. Convert Between Monitors\n\n");

  LONG choice;

  if (!readLong("Select conversion: ", &choice)) {
    return;
  }

  switch (choice) {
  case 1:
    convertDesktopToMonitor(&list);
    break;

  case 2:
    convertMonitorToDesktop(&list);
    break;

  case 3:
    convertBetweenMonitors(&list);
    break;

  default:
    printf("Invalid selection.\n");
    break;
  }
}