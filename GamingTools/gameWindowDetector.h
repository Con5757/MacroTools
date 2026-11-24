#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
typedef HWND GTWindowHandle;
#else
typedef void *GTWindowHandle;
#endif

typedef struct {
  GTWindowHandle handle;
  char title[256];
  int x;
  int y;
  int width;
  int height;
  bool visible;
} GTGameWindowInfo;
typedef struct {
  const char *filter;
  GTGameWindowInfo *windows;
  size_t capacity;
  size_t count;
} GTWindowSearch;

bool gameWindowDetectorFind(const char *titleSubstring,
                            GTGameWindowInfo *windows, size_t capacity,
                            size_t *foundCount);