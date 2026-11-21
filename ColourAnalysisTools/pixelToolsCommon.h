#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  BYTE r;
  BYTE g;
  BYTE b;
} PixelColor;

typedef struct {
  int x;
  int y;
  PixelColor color;
} PixelPoint;

typedef struct {
  int x;
  int y;
  int width;
  int height;
  PixelColor *pixels;
} PixelImage;

typedef struct {
  PixelPoint *items;
  size_t count;
  size_t capacity;
} PixelPointArray;

BOOL pixelImageCapture(int x, int y, int width, int height, PixelImage *image);
BOOL pixelImageCaptureVirtualDesktop(PixelImage *image);
void pixelImageDestroy(PixelImage *image);
BOOL pixelReadColorAt(int x, int y, PixelColor *color);

void pixelColorToHex(PixelColor color, char hex[8]);
void pixelColorToHSV(PixelColor color, double *hue, double *saturation,
                     double *value);
void pixelColorToHSL(PixelColor color, double *hue, double *saturation,
                     double *lightness);
void pixelColorToCMYK(PixelColor color, double *cyan, double *magenta,
                      double *yellow, double *black);
double pixelColorDistance(PixelColor a, PixelColor b);
BOOL pixelPointArrayAppend(PixelPointArray *array, PixelPoint point);
void pixelPointArrayDestroy(PixelPointArray *array);
BOOL pixelRegionIsValid(int x, int y, int width, int height);