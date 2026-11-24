#include "healthBarDetector.h"
#include <limits.h>
#include <stdlib.h>

bool healthBarDetectorMeasure(const GTImage *frame, GTRect region,
                              GTColor fillColor, double colorTolerance,
                              GTBarOrientation orientation, unsigned sampleStep,
                              GTHealthBarResult *result) {
  if (result == NULL || !gtRectIsValidForImage(frame, region) ||
      colorTolerance < 0.0 || colorTolerance > 765.0 || sampleStep > INT_MAX ||
      (orientation != GT_BAR_HORIZONTAL && orientation != GT_BAR_VERTICAL))
    return false;
  if (sampleStep == 0)
    sampleStep = 1;

  int axisLength =
      orientation == GT_BAR_HORIZONTAL ? region.width : region.height;
  int crossLength =
      orientation == GT_BAR_HORIZONTAL ? region.height : region.width;
  size_t axisSamples = ((size_t)axisLength + sampleStep - 1u) / sampleStep;
  size_t crossSamples = ((size_t)crossLength + sampleStep - 1u) / sampleStep;
  if (axisSamples == 0 || crossSamples == 0 ||
      axisSamples > SIZE_MAX / sizeof(size_t))
    return false;

  size_t *matchedAlongAxis = (size_t *)calloc(axisSamples, sizeof(size_t));
  if (matchedAlongAxis == NULL)
    return false;
  size_t matched = 0, total = 0;

  for (int64_t y = region.y; y < (int64_t)region.y + region.height;
       y += sampleStep) {
    for (int64_t x = region.x; x < (int64_t)region.x + region.width;
         x += sampleStep) {
      GTColor pixel;
      if (!gtImageGetPixel(frame, (int)x, (int)y, &pixel)) {
        free(matchedAlongAxis);
        return false;
      }
      if (gtColorManhattanDistance(pixel, fillColor) <= colorTolerance) {
        matched++;
        size_t axisIndex = orientation == GT_BAR_HORIZONTAL
                               ? (size_t)(x - region.x) / sampleStep
                               : (size_t)(y - region.y) / sampleStep;
        matchedAlongAxis[axisIndex]++;
      }
      total++;
    }
  }

  size_t filledAxisSamples = 0;
  for (size_t i = 0; i < axisSamples; ++i) {
    if (matchedAlongAxis[i] * 2u >= crossSamples)
      filledAxisSamples++;
  }
  free(matchedAlongAxis);

  result->matchedPixels = matched;
  result->totalPixels = total;
  result->fillRatio = (double)filledAxisSamples / (double)axisSamples;
  result->valid = total > 0;
  return result->valid;
}