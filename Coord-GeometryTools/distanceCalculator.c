#include "distanceCalculator.h"
#include <float.h>

CTStatus ctCalculateDistance(CTPoint first, CTPoint second,
                             CTDistanceMetrics *metrics) {
  if (metrics == NULL || !ctPointIsFinite(first) || !ctPointIsFinite(second)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  const double dx = fabs(second.x - first.x);
  const double dy = fabs(second.y - first.y);

  metrics->manhattan = dx + dy;
  metrics->chebyshev = dx > dy ? dx : dy;
  metrics->euclidean = hypot(dx, dy);

  if (!ctIsFinite(metrics->manhattan) || !ctIsFinite(metrics->chebyshev) ||
      !ctIsFinite(metrics->euclidean)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  return CT_STATUS_OK;
}