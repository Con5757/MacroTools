#pragma once
#include "coordinateToolsCommon.h"

typedef struct {
  double euclidean;
  double manhattan;
  double chebyshev;
} CTDistanceMetrics;

CTStatus ctCalculateDistance(CTPoint first, CTPoint second,
                             CTDistanceMetrics *metrics);