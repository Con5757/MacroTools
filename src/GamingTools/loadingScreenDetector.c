#include "loadingScreenDetector.h"
#include <stdlib.h>
#include <string.h>

bool loadingScreenDetectorInitialize(GTLoadingScreenDetector *detector) {
  if (detector == NULL)
    return false;
  memset(detector, 0, sizeof(*detector));
  return true;
}
void loadingScreenDetectorDestroy(GTLoadingScreenDetector *detector) {
  if (detector == NULL)
    return;
  free(detector->previousPixels);
  memset(detector, 0, sizeof(*detector));
}
static double frameDifference(const GTImage *a, const GTImage *b) {
  if (!gtImageIsValid(a) || !gtImageIsValid(b) || a->width != b->width ||
      a->height != b->height)
    return 255.0;
  uint64_t sum = 0;
  size_t channels = (size_t)a->width * (size_t)a->height * 3u;
  for (int y = 0; y < a->height; ++y) {
    const uint8_t *pa = a->pixels + (size_t)y * a->stride;
    const uint8_t *pb = b->pixels + (size_t)y * b->stride;
    for (size_t x = 0; x < (size_t)a->width * 3u; ++x) {
      int d = (int)pa[x] - (int)pb[x];
      sum += (uint64_t)(d < 0 ? -d : d);
    }
  }
  return (double)sum / (double)channels;
}
bool loadingScreenDetectorUpdate(GTLoadingScreenDetector *detector,
                                 const GTImage *frame, uint64_t timestampMs,
                                 double activityThreshold,
                                 uint32_t minimumActivityDurationMs,
                                 bool *loadingDetected, double *difference) {
  if (detector == NULL || loadingDetected == NULL || !gtImageIsValid(frame) ||
      activityThreshold < 0.0 || activityThreshold > 255.0 ||
      (detector->initialized && timestampMs < detector->lastUpdateMs))
    return false;

  size_t rowBytes = (size_t)frame->width * 3u;
  size_t bytes = rowBytes * (size_t)frame->height;
  bool hasComparableFrame = detector->initialized &&
                            detector->previousFrame.width == frame->width &&
                            detector->previousFrame.height == frame->height;
  double score = hasComparableFrame
                     ? frameDifference(frame, &detector->previousFrame)
                     : 0.0;

  if (!detector->initialized || detector->previousFrame.width != frame->width ||
      detector->previousFrame.height != frame->height) {
    uint8_t *copy = (uint8_t *)malloc(bytes);
    if (copy == NULL)
      return false;
    for (int y = 0; y < frame->height; ++y)
      memcpy(copy + (size_t)y * rowBytes,
             frame->pixels + (size_t)y * frame->stride, rowBytes);
    free(detector->previousPixels);
    detector->previousPixels = copy;
    detector->previousFrame =
        (GTImage){frame->width, frame->height, rowBytes, copy};
    detector->initialized = true;
    detector->activityActive = false;
    detector->activityStartMs = timestampMs;
  } else {
    uint8_t *copy = detector->previousPixels;
    for (int y = 0; y < frame->height; ++y)
      memcpy(copy + (size_t)y * rowBytes,
             frame->pixels + (size_t)y * frame->stride, rowBytes);
    detector->previousFrame.pixels = copy;
  }

  detector->lastUpdateMs = timestampMs;

  if (hasComparableFrame && score >= activityThreshold) {
    if (!detector->activityActive)
      detector->activityStartMs = timestampMs;
    detector->activityActive = true;
  } else {
    detector->activityActive = false;
  }

  *loadingDetected =
      detector->activityActive &&
      timestampMs - detector->activityStartMs >= minimumActivityDurationMs;
  if (difference != NULL)
    *difference = score;
  return true;
}