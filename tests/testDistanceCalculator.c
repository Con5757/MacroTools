#include "../src/Coord-GeometryTools/distanceCalculator.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

static int failures = 0;

static void check(bool passed, const char *name) {
  if (passed) {
    printf("[PASS] %s\n", name);
  } else {
    printf("[FAIL] %s\n", name);
    failures++;
  }
}

static bool nearlyEqual(double a, double b) { return fabs(a - b) < 1e-9; }

int main(void) {
  CTDistanceMetrics metrics = {0};
  CTStatus status;

  CTPoint origin = {0.0, 0.0};
  CTPoint point = {3.0, 4.0};

  status = ctCalculateDistance(origin, point, &metrics);

  check(status == CT_STATUS_OK && nearlyEqual(metrics.euclidean, 5.0) &&
            nearlyEqual(metrics.manhattan, 7.0) &&
            nearlyEqual(metrics.chebyshev, 4.0),
        "Standard distance calculations");

  CTPoint negative = {-3.0, -4.0};

  status = ctCalculateDistance(negative, origin, &metrics);

  check(status == CT_STATUS_OK && nearlyEqual(metrics.euclidean, 5.0) &&
            nearlyEqual(metrics.manhattan, 7.0) &&
            nearlyEqual(metrics.chebyshev, 4.0),
        "Negative coordinates");

  status = ctCalculateDistance(origin, origin, &metrics);

  check(status == CT_STATUS_OK && metrics.euclidean == 0.0 &&
            metrics.manhattan == 0.0 && metrics.chebyshev == 0.0,
        "Identical coordinates");

  status = ctCalculateDistance(origin, point, NULL);

  check(status == CT_STATUS_INVALID_ARGUMENT, "Null output pointer");

  CTPoint nanPoint = {NAN, 0.0};

  status = ctCalculateDistance(nanPoint, origin, &metrics);

  check(status == CT_STATUS_INVALID_ARGUMENT, "NaN input");

  CTPoint infinitePoint = {INFINITY, 0.0};

  status = ctCalculateDistance(infinitePoint, origin, &metrics);

  check(status == CT_STATUS_INVALID_ARGUMENT, "Infinite input");

  CTPoint extremePoint = {DBL_MAX, DBL_MAX};
  CTPoint oppositePoint = {-DBL_MAX, -DBL_MAX};

  status = ctCalculateDistance(extremePoint, oppositePoint, &metrics);

  check(status == CT_STATUS_OUT_OF_RANGE, "Numerical overflow");

  printf("\n%d test(s) failed.\n", failures);

  return failures == 0 ? 0 : 1;
}