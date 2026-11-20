#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parseInt(const char *text, int *result) {
  char *end;
  errno = 0;

  long value = strtol(text, &end, 10);
  if (errno || end == text || *end != '\0' || value < 0 || value > INT_MAX)
    return 0;

  *result = (int)value;
  return 1;
}

static void printScreenInfo(int width, int height, int x, int y) {
  printf("\nMonitor Res: %d x %d\n", width, height);
  printf("Loc/Coord: %d x %d\n", x, y);
  printf("Monitor Pos: %.2f%% x %.2f%%\n", (double)x / width * 100,
         (double)y / height * 100);
  printf("\n");
}

static int readPixel(HDC screen, int x, int y, COLORREF *color) {
  COLORREF result = GetPixel(screen, x, y);
  if (result == CLR_INVALID)
    return 0;

  *color = result;
  return 1;
}

static void printColor(COLORREF color) {
  unsigned char red = GetRValue(color);
  unsigned char green = GetGValue(color);
  unsigned char blue = GetBValue(color);

  printf("hex: #%02X%02X%02X\n", red, green, blue);
  printf("rgb: %u, %u, %u", red, green, blue);
}

static void monitorPixel(HDC screen, int x, int y, DWORD interval) {
  printf("inspecting pixel %d x %d\n", x, y);
  printf("interval: %lu ms\n", (unsigned long)interval);
  printf("ctrl+c to stop");

  while (1) {
    COLORREF color;
    if (!readPixel(screen, x, y, &color)) {
      fprintf(stderr, "failed to read pixel\n");
      break;
    }

    SYSTEMTIME time;
    GetLocalTime(&time);

    printf("[%02d:%02d:%02d] ", time.wHour, time.wMinute, time.wSecond);
    unsigned char red = GetRValue(color);
    unsigned char green = GetGValue(color);
    unsigned char blue = GetBValue(color);

    printf("#%02X%02X%02X | RGB(%u, %u, %u)\n", red, green, blue, red, green,
           blue);

    fflush(stdout);
    Sleep(interval);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Usage:\n");
    printf("\tcolorLocator sample X Y\n");
    printf("\tcolorLocator watch X Y [interval_ms]\n");
    return 1;
  }

  const char *mode = argv[1];
  int x, y;

  if (!parseInt(argv[2], &x) || !parseInt(argv[3], &y)) {
    fprintf(stdout, "invalid coords\n");
    return 1;
  }

  SetProcessDPIAware(); // needed before gathering screen dimensions

  int width = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);

  if (width <= 0 || height <= 0) {
    fprintf(stderr, "couldnt gather screen dimensions");
    return 1;
  }
  if (x >= width || y >= height) {
    fprintf(stderr,
            "Coordinates outside screen boundaries.\n"
            "Screen dimensions: %d x %d\n",
            width, height);

            return 1;
  }

  HDC screen = GetDC(NULL);
  if (!screen) {
    fprintf(stderr, "couldnt access screen data");
    return 1; 
  }

  printf("// ---------- //");
  printf("\tColor tool");
  printf("// ---------- //");

  printScreenInfo(width, height, x, y);
  if (strcmp(mode, "sample") == 0) {
    COLORREF color;

    if (!readPixel(screen, x, y, &color)) {
        fprintf(stderr, "failed to read pixel");
        ReleaseDC(NULL, screen);
        return 1;
    }

    printColor(color);
  } else if (strcmp(mode, "watch") == 0) {
    int interval = 1000;

    if (argc >= 5 && parseInt(argv[4], &interval)) {
        fprintf(stderr, "invalid interval.\n");
        ReleaseDC(NULL, screen);
        return 1;
    }

    if (interval < 1 || interval > 6000) {
      fprintf(stderr, "interval must be between 1 and 6000 ms.\n");
      ReleaseDC(NULL, screen);
      return 1;
    }

    monitorPixel(screen, x, y, (DWORD)interval);
  } else {
    fprintf(stderr, "unknown mode\n");
    ReleaseDC(NULL, screen);
    return 1;
  }

  ReleaseDC(NULL, screen);
  return 0;
}

/*
// one pixel
colorLocator sample 260 123

// one pixel a second
colorLocator watch 260 123

// 100 miliseconds or other timing
colorLocator watch 260 123 100
*/