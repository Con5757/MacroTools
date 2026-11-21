#include "colourThresholdTester.h"

#include <math.h>
#include <stdlib.h>

BOOL colourThresholdTesterTest(PixelColor expected, PixelColor actual,
                               PixelColourThreshold threshold,
                               PixelColourThresholdResult *result) {
  if (result == NULL)
    return FALSE;
  if ((threshold.mode != PIXEL_COLOUR_THRESHOLD_PER_CHANNEL &&
       threshold.mode != PIXEL_COLOUR_THRESHOLD_EUCLIDEAN) ||
      threshold.perChannelTolerance < 0 ||
      threshold.perChannelTolerance > 255 ||
      !isfinite(threshold.euclideanTolerance) ||
      threshold.euclideanTolerance < 0.0) {
    return FALSE;
  }

  result->redDifference = (int)actual.r - (int)expected.r;
  result->greenDifference = (int)actual.g - (int)expected.g;
  result->blueDifference = (int)actual.b - (int)expected.b;
  result->euclideanDistance = pixelColorDistance(expected, actual);

  if (threshold.mode == PIXEL_COLOUR_THRESHOLD_PER_CHANNEL) {
    result->matched =
        abs(result->redDifference) <= threshold.perChannelTolerance &&
        abs(result->greenDifference) <= threshold.perChannelTolerance &&
        abs(result->blueDifference) <= threshold.perChannelTolerance;
  } else {
    result->matched = result->euclideanDistance <= threshold.euclideanTolerance;
  }

  return TRUE;
}