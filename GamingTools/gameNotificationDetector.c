#include "gameNotificationDetector.h"
#include <stdlib.h>
#include <string.h>

bool gameNotificationDetectorInitialize(GTGameNotificationDetector *detector) {
  if (detector == NULL)
    return false;
  memset(detector, 0, sizeof(*detector));
  return true;
}
void gameNotificationDetectorDestroy(GTGameNotificationDetector *detector) {
  if (detector == NULL)
    return;
  free(detector->previousPixels);
  memset(detector, 0, sizeof(*detector));
}
bool gameNotificationDetectorUpdate(GTGameNotificationDetector *detector,
                                    const GTImage *frame, GTRect region,
                                    uint64_t timestampMs, double threshold,
                                    bool *detected, double *difference) {
  if (detector == NULL || detected == NULL || threshold < 0.0 ||
      threshold > 255.0 || !gtRectIsValidForImage(frame, region) ||
      (detector->initialized && timestampMs < detector->lastTimestampMs))
    return false;
  size_t stride = (size_t)region.width * 3u;
  size_t bytes = stride * (size_t)region.height;
  uint8_t *current = (uint8_t *)malloc(bytes);
  if (current == NULL)
    return false;
  for (int y = 0; y < region.height; ++y)
    memcpy(current + (size_t)y * stride,
           frame->pixels + (size_t)(region.y + y) * frame->stride +
               (size_t)region.x * 3u,
           stride);

  bool hasComparableFrame = detector->initialized &&
                            detector->previousWidth == region.width &&
                            detector->previousHeight == region.height;
  double score = 0.0;
  if (hasComparableFrame) {
    uint64_t sum = 0;
    size_t channels = bytes;
    for (size_t i = 0; i < channels; ++i) {
      int d = (int)current[i] - (int)detector->previousPixels[i];
      sum += (uint64_t)(d < 0 ? -d : d);
    }
    score = (double)sum / (double)channels;
  }

  free(detector->previousPixels);
  detector->previousPixels = current;
  detector->previousWidth = region.width;
  detector->previousHeight = region.height;
  detector->previousStride = stride;
  detector->lastTimestampMs = timestampMs;
  detector->initialized = true;
  *detected = hasComparableFrame && score >= threshold;
  if (difference != NULL)
    *difference = score;
  return true;
}