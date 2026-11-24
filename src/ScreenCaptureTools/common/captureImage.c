#define WIN32_LEAN_AND_MEAN
#include "captureImage.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

BOOL captureImageCreate(CaptureImage *image, UINT width, UINT height) {
  if (image == NULL || width == 0 || height == 0 || width > UINT_MAX / 4u) {
    return FALSE;
  }

  size_t stride = (size_t)width * 4u;
  if (height > SIZE_MAX / stride || stride > UINT_MAX) {
    return FALSE;
  }

  BYTE *pixels = (BYTE *)calloc((size_t)height, stride);
  if (pixels == NULL) {
    return FALSE;
  }

  captureImageFree(image);
  image->width = width;
  image->height = height;
  image->stride = (UINT)stride;
  image->dataSize = stride * (size_t)height;
  image->pixels = pixels;
  image->timestampMs = GetTickCount64();
  return TRUE;
}

void captureImageFree(CaptureImage *image) {
  if (image == NULL) {
    return;
  }
  free(image->pixels);
  ZeroMemory(image, sizeof(*image));
}

BOOL captureImageClone(const CaptureImage *source, CaptureImage *destination) {
  if (source == NULL || destination == NULL || source->pixels == NULL ||
      source->width == 0 || source->height == 0) {
    return FALSE;
  }
  if (!captureImageCreate(destination, source->width, source->height)) {
    return FALSE;
  }
  memcpy(destination->pixels, source->pixels, source->dataSize);
  destination->timestampMs = source->timestampMs;
  return TRUE;
}

static BOOL captureToDib(HDC sourceDc, int sourceX, int sourceY, UINT width,
                         UINT height, CaptureImage *image) {
  if (sourceDc == NULL || image == NULL || width == 0 || height == 0 ||
      width > INT_MAX || height > INT_MAX) {
    return FALSE;
  }

  HDC memoryDc = CreateCompatibleDC(sourceDc);
  if (memoryDc == NULL) {
    return FALSE;
  }

  BITMAPINFO bitmapInfo;
  ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
  bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmapInfo.bmiHeader.biWidth = (LONG)width;
  bitmapInfo.bmiHeader.biHeight = -(LONG)height;
  bitmapInfo.bmiHeader.biPlanes = 1;
  bitmapInfo.bmiHeader.biBitCount = 32;
  bitmapInfo.bmiHeader.biCompression = BI_RGB;

  void *dibPixels = NULL;
  HBITMAP dib = CreateDIBSection(sourceDc, &bitmapInfo, DIB_RGB_COLORS,
                                 &dibPixels, NULL, 0);
  if (dib == NULL || dibPixels == NULL) {
    if (dib != NULL)
      DeleteObject(dib);
    DeleteDC(memoryDc);
    return FALSE;
  }

  HGDIOBJ previousObject = SelectObject(memoryDc, dib);
  BOOL copied = BitBlt(memoryDc, 0, 0, (int)width, (int)height, sourceDc,
                       sourceX, sourceY, SRCCOPY | CAPTUREBLT);

  BOOL success = FALSE;
  if (copied && captureImageCreate(image, width, height)) {
    memcpy(image->pixels, dibPixels, image->dataSize);
    image->timestampMs = GetTickCount64();
    success = TRUE;
  }

  if (previousObject != NULL && previousObject != HGDI_ERROR) {
    SelectObject(memoryDc, previousObject);
  }
  DeleteObject(dib);
  DeleteDC(memoryDc);
  return success;
}

BOOL captureImageCaptureRect(const RECT *rect, CaptureImage *image) {
  if (rect == NULL || image == NULL || rect->right <= rect->left ||
      rect->bottom <= rect->top) {
    return FALSE;
  }

  LONGLONG width64 = (LONGLONG)rect->right - rect->left;
  LONGLONG height64 = (LONGLONG)rect->bottom - rect->top;
  if (width64 <= 0 || height64 <= 0 || width64 > UINT_MAX ||
      height64 > UINT_MAX) {
    return FALSE;
  }

  HDC screenDc = GetDC(NULL);
  if (screenDc == NULL) {
    return FALSE;
  }
  BOOL success = captureToDib(screenDc, rect->left, rect->top, (UINT)width64,
                              (UINT)height64, image);
  ReleaseDC(NULL, screenDc);
  return success;
}

BOOL captureImageCaptureWindow(HWND window, CaptureImage *image) {
  if (window == NULL || image == NULL || !IsWindow(window) ||
      IsIconic(window)) {
    return FALSE;
  }

  RECT bounds;
  if (!GetWindowRect(window, &bounds) || bounds.right <= bounds.left ||
      bounds.bottom <= bounds.top) {
    return FALSE;
  }

  LONGLONG width64 = (LONGLONG)bounds.right - bounds.left;
  LONGLONG height64 = (LONGLONG)bounds.bottom - bounds.top;
  if (width64 > INT_MAX || height64 > INT_MAX) {
    return FALSE;
  }

  UINT width = (UINT)width64;
  UINT height = (UINT)height64;
  HDC memoryDc = CreateCompatibleDC(NULL);
  if (memoryDc == NULL) {
    return FALSE;
  }

  BITMAPINFO bitmapInfo;
  ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
  bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmapInfo.bmiHeader.biWidth = (LONG)width;
  bitmapInfo.bmiHeader.biHeight = -(LONG)height;
  bitmapInfo.bmiHeader.biPlanes = 1;
  bitmapInfo.bmiHeader.biBitCount = 32;
  bitmapInfo.bmiHeader.biCompression = BI_RGB;

  void *dibPixels = NULL;
  HBITMAP dib = CreateDIBSection(memoryDc, &bitmapInfo, DIB_RGB_COLORS,
                                 &dibPixels, NULL, 0);
  if (dib == NULL || dibPixels == NULL) {
    if (dib != NULL)
      DeleteObject(dib);
    DeleteDC(memoryDc);
    return FALSE;
  }

  HGDIOBJ previousObject = SelectObject(memoryDc, dib);
  BOOL captured = PrintWindow(window, memoryDc, 0x00000002u);
  if (!captured) {
    HDC screenDc = GetDC(NULL);
    if (screenDc != NULL) {
      captured = BitBlt(memoryDc, 0, 0, (int)width, (int)height, screenDc,
                        bounds.left, bounds.top, SRCCOPY | CAPTUREBLT);
      ReleaseDC(NULL, screenDc);
    }
  }

  BOOL success = FALSE;
  if (captured && captureImageCreate(image, width, height)) {
    memcpy(image->pixels, dibPixels, image->dataSize);
    image->timestampMs = GetTickCount64();
    success = TRUE;
  }

  if (previousObject != NULL && previousObject != HGDI_ERROR) {
    SelectObject(memoryDc, previousObject);
  }
  DeleteObject(dib);
  DeleteDC(memoryDc);
  return success;
}