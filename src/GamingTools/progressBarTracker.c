#include "progressBarTracker.h"
#include <limits.h>

bool progressBarTrackerUpdate(const GTImage *frame, GTRect region,
                              GTColor fillColor, double tolerance,
                              unsigned sampleStep, GTProgressResult *result) {
  if (result == NULL || tolerance < 0.0 || tolerance > 765.0 ||
      sampleStep > INT_MAX || !gtRectIsValidForImage(frame, region))
    return false;
  if (sampleStep == 0)
    sampleStep = 1;
  size_t filled = 0, total = 0;
  for (int64_t y = region.y; y < (int64_t)region.y + region.height;
       y += sampleStep) {
    for (int64_t x = region.x; x < (int64_t)region.x + region.width;
         x += sampleStep) {
      GTColor p;
      if (!gtImageGetPixel(frame, (int)x, (int)y, &p))
        return false;
      if (gtColorManhattanDistance(p, fillColor) <= tolerance)
        filled++;
      total++;
    }
  }
  result->filledSamples = filled;
  result->totalSamples = total;
  result->progress = total ? (double)filled / (double)total : 0.0;
  result->valid = total != 0;
  return true;
}