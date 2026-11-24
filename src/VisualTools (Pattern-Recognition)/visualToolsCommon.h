#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int x, y;
} VTPoint;
typedef struct {
  int x, y, width, height;
} VTRect;
typedef struct {
  uint8_t r, g, b;
} VTRgb;
typedef struct {
  int width;
  int height;
  int stride;
  uint8_t *pixels; // packed rgb24
} VTImage;
typedef enum {
  VT_STATUS_OK = 0,
  VT_STATUS_INVALID_ARGUMENT,
  VT_STATUS_OUT_OF_MEMORY,
  VT_STATUS_CAPTURE_FAILED,
  VT_STATUS_NOT_FOUND,
  VT_STATUS_UNSUPPORTED,
  VT_STATUS_IO_ERROR
} VTStatus;

BOOL vtImageIsValid(const VTImage *image);
VTStatus vtImageCreate(VTImage *image, int width, int height);
VTStatus vtImageClone(const VTImage *source, VTImage *destination);
VTStatus vtImageCrop(const VTImage *source, VTRect region,
                     VTImage *destination);
void vtImageDestroy(VTImage *image);
BOOL vtImageGetPixel(const VTImage *image, int x, int y, VTRgb *color);
BOOL vtImageSetPixel(VTImage *image, int x, int y, VTRgb color);
VTStatus vtImageCaptureScreen(VTRect screenRegion, VTImage *destination);
VTStatus vtImageCaptureDesktop(VTImage *destination);
unsigned int vtRgbManhattanDistance(VTRgb a, VTRgb b);
unsigned int vtRgbBrightness(VTRgb color);
double vtRectIntersectionOverUnion(VTRect a, VTRect b);
BOOL vtRectIsValid(VTRect rect);
VTStatus vtImageFindTemplate(const VTImage *haystack, const VTImage *needle,
                             VTRect searchRegion, int step, double *bestScore,
                             VTPoint *bestPosition);

#ifdef __cplusplus
}
#endif
