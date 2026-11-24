#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int x;
  int y;
} GTPoint;
typedef struct {
  int x;
  int y;
  int width;
  int height;
} GTRect;
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} GTColor;
typedef struct {
  int width;
  int height;
  size_t stride;
  const uint8_t *pixels;
} GTImage;
typedef struct {
  GTColor mean;
  GTColor minimum;
  GTColor maximum;
  double meanBrightness;
  double brightnessVariance;
  size_t pixelCount;
} GTImageRegionStats;
typedef struct {
  bool found;
  GTRect bounds;
  double meanChannelDifference; // 0-255 difference
} GTImageMatch;

bool gtImageIsValid(const GTImage *image);
bool gtRectIsValidForImage(const GTImage *image, GTRect region);
bool gtImageGetPixel(const GTImage *image, int x, int y, GTColor *color);
bool gtImageMeasureRegion(const GTImage *image, GTRect region,
                          GTImageRegionStats *stats);
double gtColorManhattanDistance(GTColor a, GTColor b);
bool gtImageFindTemplate(const GTImage *image, const GTImage *templ,
                         double maximumMeanDifference, GTImageMatch *match);
bool gtImageCopyRegion(const GTImage *image, GTRect region, uint8_t **pixels,
                       size_t *stride);

#ifdef __cplusplus
}
#endif