#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <stddef.h>

typedef struct {
  UINT width;
  UINT height;
  UINT stride;
  size_t dataSize;
  BYTE *pixels;
  ULONGLONG timestampMs;
} CaptureImage;

BOOL captureImageCreate(CaptureImage *image, UINT width, UINT height);
void captureImageFree(CaptureImage *image);
BOOL captureImageClone(const CaptureImage *source, CaptureImage *destination);
BOOL captureImageCaptureRect(const RECT *rect, CaptureImage *image);
BOOL captureImageCaptureWindow(HWND window, CaptureImage *image);