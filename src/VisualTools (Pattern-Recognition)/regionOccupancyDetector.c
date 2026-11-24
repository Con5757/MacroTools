#include "regionOccupancyDetector.h"
#include <stdlib.h>

VTStatus vtRegionMeasureOccupancy(const VTImage *a, const VTImage *b, VTRect r,
                                  uint8_t threshold, double minimum,
                                  VTRegionOccupancy *out) {
  if (!vtImageIsValid(a) || !vtImageIsValid(b) || out == NULL ||
      a->width != b->width || a->height != b->height || !vtRectIsValid(r) ||
      r.x < 0 || r.y < 0 || r.x > a->width - r.width ||
      r.y > a->height - r.height || threshold == 0 || minimum < 0.0 ||
      minimum > 1.0)
    return VT_STATUS_INVALID_ARGUMENT;
  size_t active = 0;
  int minX = r.x + r.width, minY = r.y + r.height, maxX = -1, maxY = -1;
  for (int y = r.y; y < r.y + r.height; ++y) {
    const uint8_t *pa = a->pixels + (size_t)y * a->stride + (size_t)r.x * 3u,
                  *pb = b->pixels + (size_t)y * b->stride + (size_t)r.x * 3u;
    for (int x = 0; x < r.width; ++x) {
      int d = (abs((int)pa[x * 3] - (int)pb[x * 3]) +
               abs((int)pa[x * 3 + 1] - (int)pb[x * 3 + 1]) +
               abs((int)pa[x * 3 + 2] - (int)pb[x * 3 + 2])) /
              3;
      if (d >= threshold) {
        int px = r.x + x;
        ++active;
        if (px < minX)
          minX = px;
        if (px > maxX)
          maxX = px;
        if (y < minY)
          minY = y;
        if (y > maxY)
          maxY = y;
      }
    }
  }
  double occupancy = (double)active / ((double)r.width * r.height);
  out->activePixels = active;
  out->occupancy = occupancy;
  out->occupied = occupancy >= minimum && active > 0;
  out->activityBounds =
      active ? (VTRect){minX, minY, maxX - minX + 1, maxY - minY + 1}
             : (VTRect){0, 0, 0, 0};
  return VT_STATUS_OK;
}