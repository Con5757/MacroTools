#define WIN32_LEAN_AND_MEAN
#include "regionChangeDetector.h"

BOOL regionChangeDetectorInitialize(RegionChangeDetector *detector,
                                    const RECT *imageRelativeRegion,
                                    BYTE channelThreshold,
                                    double changePercentageThreshold) {
  if (detector == NULL || imageRelativeRegion == NULL ||
      imageRelativeRegion->left < 0 || imageRelativeRegion->top < 0 ||
      imageRelativeRegion->right <= imageRelativeRegion->left ||
      imageRelativeRegion->bottom <= imageRelativeRegion->top)
    return FALSE;
  ZeroMemory(detector, sizeof(*detector));
  detector->region = *imageRelativeRegion;
  detector->channelThreshold = channelThreshold;
  detector->changePercentageThreshold =
      changePercentageThreshold < 0.0
          ? 0.0
          : (changePercentageThreshold > 100.0 ? 100.0
                                               : changePercentageThreshold);
  detector->initialized = TRUE;
  return TRUE;
}

void regionChangeDetectorReset(RegionChangeDetector *detector) {
  if (detector == NULL)
    return;
  captureImageFree(&detector->previousFrame);
}
void regionChangeDetectorDestroy(RegionChangeDetector *detector) {
  regionChangeDetectorReset(detector);
  if (detector != NULL)
    detector->initialized = FALSE;
}

BOOL regionChangeDetectorUpdate(RegionChangeDetector *detector,
                                const CaptureImage *currentFrame, BOOL *changed,
                                ImageComparisonResult *comparison) {
  if (detector == NULL || !detector->initialized || currentFrame == NULL ||
      currentFrame->pixels == NULL || changed == NULL || comparison == NULL ||
      detector->region.right > (LONG)currentFrame->width ||
      detector->region.bottom > (LONG)currentFrame->height)
    return FALSE;

  ZeroMemory(comparison, sizeof(*comparison));
  *changed = FALSE;
  if (detector->previousFrame.pixels == NULL ||
      detector->previousFrame.width != currentFrame->width ||
      detector->previousFrame.height != currentFrame->height) {
    CaptureImage replacement = {0};
    if (!captureImageClone(currentFrame, &replacement))
      return FALSE;
    captureImageFree(&detector->previousFrame);
    detector->previousFrame = replacement;
    return TRUE;
  }

  if (!imageComparisonCompare(&detector->previousFrame, currentFrame,
                              &detector->region, detector->channelThreshold,
                              comparison, NULL))
    return FALSE;
  *changed =
      comparison->changedPercentage >= detector->changePercentageThreshold;

  CaptureImage replacement = {0};
  if (!captureImageClone(currentFrame, &replacement))
    return FALSE;
  captureImageFree(&detector->previousFrame);
  detector->previousFrame = replacement;
  return TRUE;
}