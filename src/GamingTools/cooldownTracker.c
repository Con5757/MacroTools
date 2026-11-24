#include "cooldownTracker.h"

bool cooldownTrackerMeasure(const GTImage *frame, GTRect region,
                            GTColor readyColor, double tolerance,
                            double readyThreshold, GTCooldownResult *result) {
  if (result == NULL || tolerance < 0.0 || tolerance > 765.0 ||
      readyThreshold < 0.0 || readyThreshold > 1.0 ||
      !gtRectIsValidForImage(frame, region))
    return false;
  size_t ready = 0, total = 0;
  for (int y = region.y; y < region.y + region.height; ++y) {
    for (int x = region.x; x < region.x + region.width; ++x) {
      GTColor pixel;
      if (!gtImageGetPixel(frame, x, y, &pixel))
        return false;
      if (gtColorManhattanDistance(pixel, readyColor) <= tolerance)
        ready++;
      total++;
    }
  }
  result->readiness = total ? (double)ready / (double)total : 0.0;
  result->ready = total > 0 && result->readiness >= readyThreshold;
  result->readyPixels = ready;
  result->sampledPixels = total;
  return total > 0;
}