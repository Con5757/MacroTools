#include "gameResolutionCalibration.h"
#include <math.h>
#include <string.h>

static bool solve3(double m[3][4], double out[3]) {
  for (int col = 0; col < 3; ++col) {
    int pivot = col;
    for (int row = col + 1; row < 3; ++row)
      if (fabs(m[row][col]) > fabs(m[pivot][col]))
        pivot = row;
    if (fabs(m[pivot][col]) < 1e-10)
      return false;
    if (pivot != col)
      for (int k = col; k < 4; ++k) {
        double temp = m[col][k];
        m[col][k] = m[pivot][k];
        m[pivot][k] = temp;
      }
    double divisor = m[col][col];
    for (int k = col; k < 4; ++k)
      m[col][k] /= divisor;
    for (int row = 0; row < 3; ++row)
      if (row != col) {
        double factor = m[row][col];
        for (int k = col; k < 4; ++k)
          m[row][k] -= factor * m[col][k];
      }
  }
  for (int i = 0; i < 3; ++i)
    out[i] = m[i][3];
  return true;
}
bool gameResolutionCalibrationFit(const GTGameCalibrationPoint *points,
                                  size_t count,
                                  GTGameAffineCalibration *calibration) {
  if (points == NULL || calibration == NULL || count < 3)
    return false;
  double normal[3][3] = {{0}};
  double rhsX[3] = {0}, rhsY[3] = {0};
  for (size_t i = 0; i < count; ++i) {
    double v[3] = {(double)points[i].source.x, (double)points[i].source.y, 1.0};
    for (int r = 0; r < 3; ++r) {
      for (int c = 0; c < 3; ++c)
        normal[r][c] += v[r] * v[c];
      rhsX[r] += v[r] * points[i].target.x;
      rhsY[r] += v[r] * points[i].target.y;
    }
  }
  double mx[3][4], my[3][4];
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c)
      mx[r][c] = my[r][c] = normal[r][c];
    mx[r][3] = rhsX[r];
    my[r][3] = rhsY[r];
  }
  double x[3], y[3];
  if (!solve3(mx, x) || !solve3(my, y))
    return false;
  calibration->a = x[0];
  calibration->b = x[1];
  calibration->c = x[2];
  calibration->d = y[0];
  calibration->e = y[1];
  calibration->f = y[2];
  calibration->calibrated = true;
  return true;
}
bool gameResolutionCalibrationMap(const GTGameAffineCalibration *calibration,
                                  GTPoint source, double *targetX,
                                  double *targetY) {
  if (calibration == NULL || !calibration->calibrated || targetX == NULL ||
      targetY == NULL)
    return false;
  *targetX =
      calibration->a * source.x + calibration->b * source.y + calibration->c;
  *targetY =
      calibration->d * source.x + calibration->e * source.y + calibration->f;
  return isfinite(*targetX) && isfinite(*targetY);
}