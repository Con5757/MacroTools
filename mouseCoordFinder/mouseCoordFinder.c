#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  DWORD interval = 100;
  if (argc > 1) {
    char *end;
    errno = 0;

    unsigned long value = strtoul(argv[1], &end, 10);
    if (errno || end == argv[1] || *end != '\0' || value < 1 || value > 60000) {
      printf("Invalid interval. Use 1-60000 ms.\n");
      return 1;
    }

    interval = (DWORD)value;
  }

  POINT cursor;
  HMONITOR previousMonitor = NULL;
  printf("Mouse tracker started. Press Ctrl+C to stop.\n\n");

  while (1) {
    if (!GetCursorPos(&cursor)) {
      printf("Failed to retrieve cursor position.\n");
      break;
    }

    HMONITOR currentMonitor =
        MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXA monitorInfo = {0};
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (currentMonitor &&
        GetMonitorInfoA(currentMonitor, (MONITORINFO *)&monitorInfo)) {
      if (previousMonitor != currentMonitor) {
        if (previousMonitor == NULL) {
          printf("[INITIAL MONITOR] %s\n", monitorInfo.szDevice);
        } else {
          printf("\n[MONITOR SWITCH] Now on %s\n", monitorInfo.szDevice);
        }

        previousMonitor = currentMonitor;
      }

      LONG localX = cursor.x - monitorInfo.rcMonitor.left;
      LONG localY = cursor.y - monitorInfo.rcMonitor.top;

      printf("[%s] Screen: (%ld, %ld) | Local: (%ld, %ld)\n",
             monitorInfo.szDevice, cursor.x, cursor.y, localX, localY);
    } else {
      printf("Screen: (%ld, %ld) | Monitor unavailable\n", cursor.x, cursor.y);
    }

    fflush(stdout);
    Sleep(interval);
  }

  return 0;
}