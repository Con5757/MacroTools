#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  GTPoint source;
  GTPoint target;
} GTGameCalibrationPoint;
typedef struct {
  double a, b, c;
  double d, e, f;
  bool calibrated;
} GTGameAffineCalibration;

bool gameResolutionCalibrationFit(const GTGameCalibrationPoint *points,
                                  size_t count,
                                  GTGameAffineCalibration *calibration);
bool gameResolutionCalibrationMap(const GTGameAffineCalibration *calibration,
                                  GTPoint source, double *targetX,
                                  double *targetY);