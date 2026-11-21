#define WIN32_LEAN_AND_MEAN
#include "monitorEnum.h"
#include <Windows.h>
#include <stdio.h>

static BOOL CALLBACK monitorEnumCallback(HMONITOR monitor, HDC dc, LPRECT rect,
                                         LPARAM data) {
  MONITORINFOEXA info;
  int *count = (int *)data;

  ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);

  if (GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
    (*count)++;
    printf("%d: %s\n", *count, info.szDevice);
  }

  return TRUE;
}

void monitorEnum(void) {
  int monitorCount = 0;

  printf("Monitor Enum\n");
  if (!EnumDisplayMonitors(NULL, NULL, monitorEnumCallback,
                           (LPARAM)&monitorCount)) {
    printf("monitor enum failed\n");
    return;
  }

  printf("total monitors: %d\n", monitorCount);
}