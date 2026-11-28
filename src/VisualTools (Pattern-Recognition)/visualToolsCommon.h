#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int VTBool;
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
  uint8_t *pixels;
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

VTBool vtImageIsValid(const VTImage *image);
VTStatus vtImageCreate(VTImage *image, int width, int height);
VTStatus vtImageClone(const VTImage *source, VTImage *destination);
VTStatus vtImageCrop(const VTImage *source, VTRect region,
                     VTImage *destination);
void vtImageDestroy(VTImage *image);
VTBool vtImageGetPixel(const VTImage *image, int x, int y, VTRgb *color);
VTBool vtImageSetPixel(VTImage *image, int x, int y, VTRgb color);
VTStatus vtImageCaptureScreen(VTRect screenRegion, VTImage *destination);
VTStatus vtImageCaptureDesktop(VTImage *destination);
unsigned int vtRgbManhattanDistance(VTRgb a, VTRgb b);
unsigned int vtRgbBrightness(VTRgb color);
double vtRectIntersectionOverUnion(VTRect a, VTRect b);
VTBool vtRectIsValid(VTRect rect);
VTStatus vtImageFindTemplate(const VTImage *haystack, const VTImage *needle,
                             VTRect searchRegion, int step, double *bestScore,
                             VTPoint *bestPosition);

#ifdef __cplusplus
}
#endif