#include "coordinateCalibrationTool.h"
#include <float.h>
#include <math.h>

CTStatus
ctInitializeCoordinateCalibration(const CTPoint source[3],
                                  const CTPoint target[3],
                                  CTCoordinateCalibration *calibration) {
  if (source == NULL || target == NULL || calibration == NULL) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  for (size_t i = 0; i < 3; ++i) {
    if (!ctPointIsFinite(source[i]) || !ctPointIsFinite(target[i])) {
      return CT_STATUS_INVALID_ARGUMENT;
    }
  }

  const double x0 = source[0].x, y0 = source[0].y;
  const double x1 = source[1].x, y1 = source[1].y;
  const double x2 = source[2].x, y2 = source[2].y;
  const double determinant = x0 * (y1 - y2) + x1 * (y2 - y0) + x2 * (y0 - y1);

  double scale = 1.0;
  for (size_t i = 0; i < 3; ++i) {
    const double ax = fabs(source[i].x);
    const double ay = fabs(source[i].y);
    if (ax > scale)
      scale = ax;
    if (ay > scale)
      scale = ay;
  }

  const double tolerance = 16.0 * DBL_EPSILON * scale * scale;
  if (!ctIsFinite(determinant) || fabs(determinant) <= tolerance) {
    return CT_STATUS_DEGENERATE_CALIBRATION;
  }

  CTCoordinateCalibration result;
  result.a = (target[0].x * (y1 - y2) + target[1].x * (y2 - y0) +
              target[2].x * (y0 - y1)) /
             determinant;
  result.b = (target[0].x * (x2 - x1) + target[1].x * (x0 - x2) +
              target[2].x * (x1 - x0)) /
             determinant;
  result.c =
      (target[0].x * (x1 * y2 - x2 * y1) + target[1].x * (x2 * y0 - x0 * y2) +
       target[2].x * (x0 * y1 - x1 * y0)) /
      determinant;
  result.d = (target[0].y * (y1 - y2) + target[1].y * (y2 - y0) +
              target[2].y * (y0 - y1)) /
             determinant;
  result.e = (target[0].y * (x2 - x1) + target[1].y * (x0 - x2) +
              target[2].y * (x1 - x0)) /
             determinant;
  result.f =
      (target[0].y * (x1 * y2 - x2 * y1) + target[1].y * (x2 * y0 - x0 * y2) +
       target[2].y * (x0 * y1 - x1 * y0)) /
      determinant;
  result.initialized = true;

  if (!ctIsFinite(result.a) || !ctIsFinite(result.b) || !ctIsFinite(result.c) ||
      !ctIsFinite(result.d) || !ctIsFinite(result.e) || !ctIsFinite(result.f)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  *calibration = result;
  return CT_STATUS_OK;
}
CTStatus
ctApplyCoordinateCalibration(const CTCoordinateCalibration *calibration,
                             CTPoint source, CTPoint *target) {
  if (calibration == NULL || target == NULL || !calibration->initialized ||
      !ctPointIsFinite(source)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  CTPoint result = {
      calibration->a * source.x + calibration->b * source.y + calibration->c,
      calibration->d * source.x + calibration->e * source.y + calibration->f};

  if (!ctPointIsFinite(result)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  *target = result;
  return CT_STATUS_OK;
}