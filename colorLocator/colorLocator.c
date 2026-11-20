#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MONITORS 16
#define DEFAULT_WATCH_INTERVAL 1000
#define DEFAULT_MOUSE_INTERVAL 50
#define MIN_INTERVAL 1
#define MAX_INTERVAL 6000

typedef struct {
  RECT bounds;
  char device[CCHDEVICENAME];
  int isPrimary;
} MonitorInfo;

typedef struct {
  MonitorInfo items[MAX_MONITORS];
  int count;
} MonitorList;

static int parseInt(const char *text, int *result) {
  char *end = NULL;
  errno = 0;

  long value = strtol(text, &end, 10);

  if (errno != 0 || end == text || *end != '\0' || value < 0 ||
      value > INT_MAX) {
    return 0;
  }

  *result = (int)value;
  return 1;
}

static BOOL CALLBACK collectMonitor(HMONITOR hMonitor, HDC hdcMonitor,
                                    LPRECT monitorRect, LPARAM userData) {
  (void)hdcMonitor;
  (void)monitorRect;

  MonitorList *list = (MonitorList *)userData;

  if (list->count >= MAX_MONITORS) {
    return TRUE;
  }

  MONITORINFOEXA info = {0};
  info.cbSize = sizeof(info);

  if (!GetMonitorInfoA(hMonitor, (LPMONITORINFO)&info)) {
    return TRUE;
  }

  MonitorInfo *monitor = &list->items[list->count++];
  monitor->bounds = info.rcMonitor;
  strncpy(monitor->device, info.szDevice, CCHDEVICENAME - 1);
  monitor->device[CCHDEVICENAME - 1] = '\0';
  monitor->isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;

  return TRUE;
}

static int chooseMonitor(const MonitorList *list, int *selected) {
  printf("\nAvailable monitors:\n\n");

  for (int i = 0; i < list->count; i++) {
    const MonitorInfo *monitor = &list->items[i];
    int width = (int)(monitor->bounds.right - monitor->bounds.left);
    int height = (int)(monitor->bounds.bottom - monitor->bounds.top);

    printf("%d. %s | %d x %d | Position: %ld, %ld%s\n", i + 1, monitor->device,
           width, height, monitor->bounds.left, monitor->bounds.top,
           monitor->isPrimary ? " [Primary]" : "");
  }

  char input[32];
  int choice;

  printf("\nSelect monitor: ");
  fflush(stdout);

  if (fgets(input, sizeof(input), stdin) == NULL) {
    fprintf(stderr, "Failed to read monitor selection.\n");
    return 0;
  }

  input[strcspn(input, "\r\n")] = '\0';

  if (!parseInt(input, &choice) || choice < 1 || choice > list->count) {
    fprintf(stderr, "Invalid monitor selection.\n");
    return 0;
  }

  *selected = choice - 1;
  return 1;
}

static void printMonitorInfo(const MonitorInfo *monitor, int width,
                             int height) {
  printf("Monitor: %s%s\n", monitor->device,
         monitor->isPrimary ? " [Primary]" : "");
  printf("Monitor Res: %d x %d\n", width, height);
  printf("Monitor Origin: %ld x %ld\n", monitor->bounds.left,
         monitor->bounds.top);
}

static void printScreenInfo(const MonitorInfo *monitor, int width, int height,
                            int x, int y, int screenX, int screenY) {
  printMonitorInfo(monitor, width, height);
  printf("Monitor Coord: %d x %d\n", x, y);
  printf("Desktop Coord: %d x %d\n", screenX, screenY);
  printf("Monitor Pos: %.2f%% x %.2f%%\n", (double)x / width * 100.0,
         (double)y / height * 100.0);
  printf("\n");
}

static int readPixel(HDC screen, int x, int y, COLORREF *color) {
  COLORREF result = GetPixel(screen, x, y);

  if (result == CLR_INVALID) {
    return 0;
  }

  *color = result;
  return 1;
}

static void printColor(COLORREF color) {
  unsigned int red = GetRValue(color);
  unsigned int green = GetGValue(color);
  unsigned int blue = GetBValue(color);

  printf("hex: #%02X%02X%02X\n", red, green, blue);
  printf("rgb: %u, %u, %u\n", red, green, blue);
}

static int monitorPixel(HDC screen, int x, int y, int screenX, int screenY,
                        DWORD interval) {
  printf("Watching monitor pixel %d x %d\n", x, y);
  printf("Desktop coordinate: %d x %d\n", screenX, screenY);
  printf("Interval: %lu ms\n", (unsigned long)interval);
  printf("Press Ctrl+C to stop.\n\n");

  while (1) {
    COLORREF color;

    if (!readPixel(screen, screenX, screenY, &color)) {
      fprintf(stderr, "Failed to read pixel.\n");
      return 0;
    }

    SYSTEMTIME time;
    GetLocalTime(&time);

    unsigned int red = GetRValue(color);
    unsigned int green = GetGValue(color);
    unsigned int blue = GetBValue(color);

    printf("[%02u:%02u:%02u] #%02X%02X%02X | RGB(%u, %u, %u)\n",
           (unsigned int)time.wHour, (unsigned int)time.wMinute,
           (unsigned int)time.wSecond, red, green, blue, red, green, blue);

    fflush(stdout);
    Sleep(interval);
  }

  return 1;
}

static int trackMouse(HDC screen, const MonitorInfo *monitor, DWORD interval) {
  int previousX = 0;
  int previousY = 0;
  COLORREF previousColor = 0;
  int havePreviousSample = 0;
  int outsideMessagePrinted = 0;

  printf("\nMouse tracking monitor: %s\n", monitor->device);
  printf("Sampling interval: %lu ms\n", (unsigned long)interval);
  printf("Move the cursor over this monitor. Press Ctrl+C to stop.\n\n");

  while (1) {
    POINT cursor;

    if (!GetCursorPos(&cursor)) {
      fprintf(stderr, "Failed to get cursor position.\n");
      return 0;
    }

    if (!PtInRect(&monitor->bounds, cursor)) {
      if (!outsideMessagePrinted) {
        printf("Cursor is outside the selected monitor.\n");
        fflush(stdout);
        outsideMessagePrinted = 1;
      }

      havePreviousSample = 0;
      Sleep(interval);
      continue;
    }

    outsideMessagePrinted = 0;

    int monitorX = (int)(cursor.x - monitor->bounds.left);
    int monitorY = (int)(cursor.y - monitor->bounds.top);
    COLORREF color;

    if (!readPixel(screen, cursor.x, cursor.y, &color)) {
      fprintf(stderr, "Failed to read pixel under the cursor.\n");
      return 0;
    }

    if (!havePreviousSample || monitorX != previousX || monitorY != previousY ||
        color != previousColor) {
      SYSTEMTIME time;
      GetLocalTime(&time);

      unsigned int red = GetRValue(color);
      unsigned int green = GetGValue(color);
      unsigned int blue = GetBValue(color);

      printf("[%02u:%02u:%02u] Monitor: %d x %d | Desktop: %ld x %ld | "
             "#%02X%02X%02X | RGB(%u, %u, %u)\n",
             (unsigned int)time.wHour, (unsigned int)time.wMinute,
             (unsigned int)time.wSecond, monitorX, monitorY, cursor.x, cursor.y,
             red, green, blue, red, green, blue);

      fflush(stdout);

      previousX = monitorX;
      previousY = monitorY;
      previousColor = color;
      havePreviousSample = 1;
    }

    Sleep(interval);
  }

  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage:\n"
                    "  colorLocator sample X Y\n"
                    "  colorLocator watch X Y [interval_ms]\n"
                    "  colorLocator mouse [interval_ms]\n");
    return 1;
  }

  const char *mode = argv[1];
  int isSample = strcmp(mode, "sample") == 0;
  int isWatch = strcmp(mode, "watch") == 0;
  int isMouse = strcmp(mode, "mouse") == 0;

  if (!isSample && !isWatch && !isMouse) {
    fprintf(stderr, "Unknown mode: %s\n", mode);
    return 1;
  }

  if ((isSample && argc != 4) || (isWatch && (argc < 4 || argc > 5)) ||
      (isMouse && (argc < 2 || argc > 3))) {
    fprintf(stderr, "Usage:\n"
                    "  colorLocator sample X Y\n"
                    "  colorLocator watch X Y [interval_ms]\n"
                    "  colorLocator mouse [interval_ms]\n");
    return 1;
  }

  int x = 0;
  int y = 0;

  if (!isMouse && (!parseInt(argv[2], &x) || !parseInt(argv[3], &y))) {
    fprintf(stderr, "Invalid coordinates. Use non-negative integers.\n");
    return 1;
  }

  int interval = isMouse ? DEFAULT_MOUSE_INTERVAL : DEFAULT_WATCH_INTERVAL;

  if ((isWatch && argc == 5 && !parseInt(argv[4], &interval)) ||
      (isMouse && argc == 3 && !parseInt(argv[2], &interval))) {
    fprintf(stderr, "Invalid interval.\n");
    return 1;
  }

  if ((isWatch || isMouse) &&
      (interval < MIN_INTERVAL || interval > MAX_INTERVAL)) {
    fprintf(stderr, "Interval must be between %d and %d ms.\n", MIN_INTERVAL,
            MAX_INTERVAL);
    return 1;
  }

  SetProcessDPIAware();

  MonitorList monitors = {0};

  if (!EnumDisplayMonitors(NULL, NULL, collectMonitor, (LPARAM)&monitors)) {
    fprintf(stderr, "Failed to enumerate monitors.\n");
    return 1;
  }

  if (monitors.count == 0) {
    fprintf(stderr, "No monitors found.\n");
    return 1;
  }

  int selectedMonitor;

  if (!chooseMonitor(&monitors, &selectedMonitor)) {
    return 1;
  }

  const MonitorInfo *monitor = &monitors.items[selectedMonitor];
  int width = (int)(monitor->bounds.right - monitor->bounds.left);
  int height = (int)(monitor->bounds.bottom - monitor->bounds.top);

  if (!isMouse && (x >= width || y >= height)) {
    fprintf(stderr,
            "Coordinates are outside the selected monitor.\n"
            "Monitor dimensions: %d x %d\n",
            width, height);
    return 1;
  }

  HDC screen = GetDC(NULL);

  if (screen == NULL) {
    fprintf(stderr, "Could not access the screen device context.\n");
    return 1;
  }

  printf("// ---------------- //\n");
  printf("      Color tool\n");
  printf("// ---------------- //\n");

  int success = 1;

  if (isMouse) {
    printMonitorInfo(monitor, width, height);
    success = trackMouse(screen, monitor, (DWORD)interval);
  } else {
    int screenX = (int)monitor->bounds.left + x;
    int screenY = (int)monitor->bounds.top + y;

    printScreenInfo(monitor, width, height, x, y, screenX, screenY);

    if (isSample) {
      COLORREF color;

      if (!readPixel(screen, screenX, screenY, &color)) {
        fprintf(stderr, "Failed to read pixel.\n");
        success = 0;
      } else {
        printColor(color);
      }
    } else {
      success = monitorPixel(screen, x, y, screenX, screenY, (DWORD)interval);
    }
  }

  ReleaseDC(NULL, screen);
  return success ? 0 : 1;
}

/*
Examples:

  colorLocator sample 260 123
  colorLocator watch 260 123
  colorLocator watch 260 123 100
  colorLocator mouse
  colorLocator mouse 25

Coordinates are relative to the monitor selected at startup.
Mouse mode tracks the cursor only while it is over the selected monitor.
It prints when the cursor position or sampled pixel color changes.
*/
