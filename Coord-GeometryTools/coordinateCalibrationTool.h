#pragma once
#include "coordinateToolsCommon.h"

typedef struct {
  double a;
  double b;
  double c;
  double d;
  double e;
  double f;
  bool initialized;
} CTCoordinateCalibration;

/*
builds a 2d affine calib from the no collinear source points
the mapping for their corresponding points are:
target.x = a*x + b*y + c
target.y = d*x + e*y + f
*/
CTStatus
ctInitializeCoordinateCalibration(const CTPoint source[3],
                                  const CTPoint target[3],
                                  CTCoordinateCalibration *calibration);
CTStatus
ctApplyCoordinateCalibration(const CTCoordinateCalibration *calibration,
                             CTPoint source, CTPoint *target);