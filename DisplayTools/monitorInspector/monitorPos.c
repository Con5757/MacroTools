#define WIN32_LEAN_AND_MEAN
#include "monitorPos.h"
#include <Windows.h>
#include <stdio.h>

static BOOL CALLBACK monitorPosCallback(HMONITOR monitor, HDC dc, LPRECT rect,
                                        LPARAM data) {
  MONITORINFOEXA info;

  ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
    return TRUE;
  }

  printf("%s: X=%ld Y=%ld Width=%ld Height=%ld\n", info.szDevice,
         info.rcMonitor.left, info.rcMonitor.top,
         info.rcMonitor.right - info.rcMonitor.left,
         info.rcMonitor.bottom - info.rcMonitor.top);

  return TRUE;
}

void monitorPos(void) {
  printf("\nMonitor Position\n");

  if (!EnumDisplayMonitors(NULL, NULL, monitorPosCallback, 0)) {
    printf("Position retrieval failed.\n");
  }
}