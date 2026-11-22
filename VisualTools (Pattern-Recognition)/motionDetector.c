#include "motionDetector.h"
#include <stdlib.h>

VTStatus vtDetectMotion(const VTImage *a, const VTImage *b,
                        uint8_t pixelThreshold, double ratioThreshold,
                        VTMotionResult *out, VTImage *mask) {
  if (!vtImageIsValid(a) || !vtImageIsValid(b) || out == NULL ||
      a->width != b->width || a->height != b->height || pixelThreshold == 0 ||
      ratioThreshold < 0.0 || ratioThreshold > 1.0)
    return VT_STATUS_INVALID_ARGUMENT;
  VTImage temp = {0};
  if (mask) {
    VTStatus s = vtImageCreate(&temp, a->width, a->height);
    if (s != VT_STATUS_OK)
      return s;
  }
  size_t changed = 0;
  int minX = a->width, minY = a->height, maxX = -1, maxY = -1;
  for (int y = 0; y < a->height; ++y) {
    const uint8_t *pa = a->pixels + (size_t)y * a->stride,
                  *pb = b->pixels + (size_t)y * b->stride;
    uint8_t *pm = mask ? temp.pixels + (size_t)y * temp.stride : NULL;
    for (int x = 0; x < a->width; ++x) {
      int d0 = abs((int)pa[x * 3] - (int)pb[x * 3]),
          d1 = abs((int)pa[x * 3 + 1] - (int)pb[x * 3 + 1]),
          d2 = abs((int)pa[x * 3 + 2] - (int)pb[x * 3 + 2]);
      BOOL hit = (d0 + d1 + d2) / 3 >= pixelThreshold;
      if (pm) {
        uint8_t v = hit ? 255u : 0u;
        pm[x * 3] = pm[x * 3 + 1] = pm[x * 3 + 2] = v;
      }
      if (hit) {
        ++changed;
        if (x < minX)
          minX = x;
        if (x > maxX)
          maxX = x;
        if (y < minY)
          minY = y;
        if (y > maxY)
          maxY = y;
      }
    }
  }
  double ratio = (double)changed / ((double)a->width * a->height);
  out->changedPixels = changed;
  out->changedRatio = ratio;
  out->motionDetected = ratio >= ratioThreshold && changed > 0;
  out->bounds = changed ? (VTRect){minX, minY, maxX - minX + 1, maxY - minY + 1}
                        : (VTRect){0, 0, 0, 0};
  if (mask) {
    vtImageDestroy(mask);
    *mask = temp;
  }
  return VT_STATUS_OK;
}