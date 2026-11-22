#include "bezierPathGenerator.h"

CTStatus ctGenerateBezierPath(CTPoint p0, CTPoint p1, CTPoint p2, CTPoint p3,
                              size_t sampleCount, CTPoint *points,
                              size_t capacity, size_t *requiredCount) {
  if (requiredCount == NULL || sampleCount < 2 || !ctPointIsFinite(p0) ||
      !ctPointIsFinite(p1) || !ctPointIsFinite(p2) || !ctPointIsFinite(p3) ||
      (points == NULL && capacity != 0)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  *requiredCount = sampleCount;
  if (points == NULL || capacity < sampleCount) {
    return CT_STATUS_BUFFER_TOO_SMALL;
  }

  for (size_t i = 0; i < sampleCount; ++i) {
    const double t = (double)i / (double)(sampleCount - 1);
    const double u = 1.0 - t;
    const double uu = u * u;
    const double tt = t * t;
    const double w0 = uu * u;
    const double w1 = 3.0 * uu * t;
    const double w2 = 3.0 * u * tt;
    const double w3 = tt * t;

    points[i].x = w0 * p0.x + w1 * p1.x + w2 * p2.x + w3 * p3.x;
    points[i].y = w0 * p0.y + w1 * p1.y + w2 * p2.y + w3 * p3.y;

    if (!ctPointIsFinite(points[i])) {
      return CT_STATUS_OUT_OF_RANGE;
    }
  }

  points[0] = p0;
  points[sampleCount - 1] = p3;
  return CT_STATUS_OK;
}