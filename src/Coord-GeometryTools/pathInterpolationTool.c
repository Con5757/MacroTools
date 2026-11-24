#include "pathInterpolationTool.h"
#include <stdint.h>

CTStatus ctInterpolatePath(CTPoint start, CTPoint end, size_t segments,
                           CTPoint *points, size_t capacity,
                           size_t *requiredCount) {
  if (requiredCount == NULL || !ctPointIsFinite(start) ||
      !ctPointIsFinite(end) || segments == 0 || segments == SIZE_MAX ||
      (points == NULL && capacity != 0)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  const size_t needed = segments + 1;
  *requiredCount = needed;

  if (points == NULL || capacity < needed) {
    return CT_STATUS_BUFFER_TOO_SMALL;
  }

  for (size_t i = 0; i < needed; ++i) {
    const double t = (double)i / (double)segments;
    points[i].x = start.x + (end.x - start.x) * t;
    points[i].y = start.y + (end.y - start.y) * t;
    if (!ctPointIsFinite(points[i])) {
      return CT_STATUS_OUT_OF_RANGE;
    }
  }

  return CT_STATUS_OK;
}