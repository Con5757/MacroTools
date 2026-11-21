#include "colourDifferenceAnalyzer.h"

#include <math.h>
#include <stdlib.h>

static double srgbToLinear(double value) {
  value /= 255.0;
  return value <= 0.04045 ? value / 12.92 : pow((value + 0.055) / 1.055, 2.4);
}

static void rgbToLab(PixelColor color, double *l, double *a, double *b) {
  double r = srgbToLinear(color.r);
  double g = srgbToLinear(color.g);
  double blue = srgbToLinear(color.b);

  // D65 ref white
  double x = (0.4124564 * r + 0.3575761 * g + 0.1804375 * blue) / 0.95047;
  double y = (0.2126729 * r + 0.7151522 * g + 0.0721750 * blue);
  double z = (0.0193339 * r + 0.1191920 * g + 0.9503041 * blue) / 1.08883;

  double fx = x > 0.008856451679 ? cbrt(x) : (7.787037037 * x + 16.0 / 116.0);
  double fy = y > 0.008856451679 ? cbrt(y) : (7.787037037 * y + 16.0 / 116.0);
  double fz = z > 0.008856451679 ? cbrt(z) : (7.787037037 * z + 16.0 / 116.0);

  *l = 116.0 * fy - 16.0;
  *a = 500.0 * (fx - fy);
  *b = 200.0 * (fy - fz);
}

void colourDifferenceAnalyzerCompare(PixelColor first, PixelColor second,
                                     PixelColourDifference *difference) {
  if (difference == NULL)
    return;

  int dr = (int)first.r - (int)second.r;
  int dg = (int)first.g - (int)second.g;
  int db = (int)first.b - (int)second.b;

  difference->redDifference = dr;
  difference->greenDifference = dg;
  difference->blueDifference = db;
  difference->rgbEuclideanDistance =
      sqrt((double)dr * dr + (double)dg * dg + (double)db * db);
  difference->rgbManhattanDistance = (double)abs(dr) + abs(dg) + abs(db);
  difference->normalizedDifferencePercent =
      difference->rgbEuclideanDistance / sqrt(3.0 * 255.0 * 255.0) * 100.0;

  double l1, a1, b1, l2, a2, b2;
  rgbToLab(first, &l1, &a1, &b1);
  rgbToLab(second, &l2, &a2, &b2);
  difference->deltaE76 = sqrt((l1 - l2) * (l1 - l2) + (a1 - a2) * (a1 - a2) +
                              (b1 - b2) * (b1 - b2));
}