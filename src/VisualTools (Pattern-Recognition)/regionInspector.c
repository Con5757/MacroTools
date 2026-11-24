#include "regionInspector.h"
#include <limits.h>

VTStatus vtRegionInspect(const VTImage *image, VTRect region,
                         VTRegionInspection *out) {
  if (!vtImageIsValid(image) || out == NULL || !vtRectIsValid(region) ||
      region.x < 0 || region.y < 0 || region.x > image->width - region.width ||
      region.y > image->height - region.height)
    return VT_STATUS_INVALID_ARGUMENT;
  uint64_t sums[3] = {0, 0, 0}, brightnessSum = 0;
  VTRgb min = {255, 255, 255}, max = {0, 0, 0};
  for (int y = region.y; y < region.y + region.height; ++y) {
    const uint8_t *p = image->pixels + (size_t)y * (size_t)image->stride +
                       (size_t)region.x * 3u;
    for (int x = 0; x < region.width; ++x, p += 3) {
      VTRgb c = {p[0], p[1], p[2]};
      sums[0] += c.r;
      sums[1] += c.g;
      sums[2] += c.b;
      brightnessSum += vtRgbBrightness(c);
      if (c.r < min.r)
        min.r = c.r;
      if (c.g < min.g)
        min.g = c.g;
      if (c.b < min.b)
        min.b = c.b;
      if (c.r > max.r)
        max.r = c.r;
      if (c.g > max.g)
        max.g = c.g;
      if (c.b > max.b)
        max.b = c.b;
    }
  }
  size_t count = (size_t)region.width * (size_t)region.height;
  out->region = region;
  out->pixelCount = count;
  out->average = (VTRgb){(uint8_t)(sums[0] / count), (uint8_t)(sums[1] / count),
                         (uint8_t)(sums[2] / count)};
  out->minimum = min;
  out->maximum = max;
  out->averageBrightness = (double)brightnessSum / count;
  return VT_STATUS_OK;
}