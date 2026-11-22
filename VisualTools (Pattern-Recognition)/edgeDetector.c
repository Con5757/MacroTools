#include "edgeDetector.h"
#include <stdlib.h>

static int grayAt(const VTImage *im, int x, int y) {
  const uint8_t *p =
      im->pixels + (size_t)y * (size_t)im->stride + (size_t)x * 3u;
  return (299 * p[0] + 587 * p[1] + 114 * p[2]) / 1000;
}
VTStatus vtDetectEdges(const VTImage *src, unsigned int threshold,
                       VTImage *mask, size_t *edgePixels) {
  if (!vtImageIsValid(src) || mask == NULL || edgePixels == NULL ||
      threshold > 2040u)
    return VT_STATUS_INVALID_ARGUMENT;
  VTImage out = {0};
  VTStatus s = vtImageCreate(&out, src->width, src->height);
  if (s != VT_STATUS_OK)
    return s;
  size_t count = 0;
  for (int y = 1; y < src->height - 1; ++y)
    for (int x = 1; x < src->width - 1; ++x) {
      int gx = -grayAt(src, x - 1, y - 1) + grayAt(src, x + 1, y - 1) -
               2 * grayAt(src, x - 1, y) + 2 * grayAt(src, x + 1, y) -
               grayAt(src, x - 1, y + 1) + grayAt(src, x + 1, y + 1);
      int gy = -grayAt(src, x - 1, y - 1) - 2 * grayAt(src, x, y - 1) -
               grayAt(src, x + 1, y - 1) + grayAt(src, x - 1, y + 1) +
               2 * grayAt(src, x, y + 1) + grayAt(src, x + 1, y + 1);
      unsigned int mag = (unsigned int)(abs(gx) + abs(gy));
      uint8_t v = mag >= threshold ? 255u : 0u;
      uint8_t *p = out.pixels + (size_t)y * (size_t)out.stride + (size_t)x * 3u;
      p[0] = p[1] = p[2] = v;
      if (v)
        ++count;
    }
  vtImageDestroy(mask);
  *mask = out;
  *edgePixels = count;
  return VT_STATUS_OK;
}