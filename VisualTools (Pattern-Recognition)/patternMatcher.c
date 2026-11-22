#include "patternMatcher.h"

VTStatus vtPatternFind(const VTImage *image, VTRect region,
                       const VTPatternPoint *points, size_t count,
                       VTPatternMatch *out) {
  if (!vtImageIsValid(image) || points == NULL || count == 0 || out == NULL ||
      !vtRectIsValid(region) || region.x < 0 || region.y < 0 ||
      region.x > image->width - region.width ||
      region.y > image->height - region.height)
    return VT_STATUS_INVALID_ARGUMENT;
  int maxX = 0, maxY = 0;
  for (size_t i = 0; i < count; ++i) {
    if (points[i].x < 0 || points[i].y < 0)
      return VT_STATUS_INVALID_ARGUMENT;
    if (points[i].x > maxX)
      maxX = points[i].x;
    if (points[i].y > maxY)
      maxY = points[i].y;
  }
  if (maxX >= region.width || maxY >= region.height)
    return VT_STATUS_INVALID_ARGUMENT;
  size_t best = 0;
  VTPoint bestPos = {0, 0};
  for (int y = region.y; y <= region.y + region.height - 1 - maxY; ++y) {
    for (int x = region.x; x <= region.x + region.width - 1 - maxX; ++x) {
      size_t matches = 0;
      for (size_t i = 0; i < count; ++i) {
        VTRgb actual;
        vtImageGetPixel(image, x + points[i].x, y + points[i].y, &actual);
        if (actual.r >= (points[i].color.r > points[i].tolerance
                             ? points[i].color.r - points[i].tolerance
                             : 0) &&
            actual.r <= (points[i].color.r + points[i].tolerance < 255
                             ? points[i].color.r + points[i].tolerance
                             : 255) &&
            actual.g >= (points[i].color.g > points[i].tolerance
                             ? points[i].color.g - points[i].tolerance
                             : 0) &&
            actual.g <= (points[i].color.g + points[i].tolerance < 255
                             ? points[i].color.g + points[i].tolerance
                             : 255) &&
            actual.b >= (points[i].color.b > points[i].tolerance
                             ? points[i].color.b - points[i].tolerance
                             : 0) &&
            actual.b <= (points[i].color.b + points[i].tolerance < 255
                             ? points[i].color.b + points[i].tolerance
                             : 255))
          ++matches;
      }
      if (matches > best) {
        best = matches;
        bestPos = (VTPoint){x, y};
      }
      if (best == count)
        break;
    }
    if (best == count)
      break;
  }
  out->anchor = bestPos;
  out->confidence = (double)best / (double)count;
  return best ? VT_STATUS_OK : VT_STATUS_NOT_FOUND;
}