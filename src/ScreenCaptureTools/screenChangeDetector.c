#define WIN32_LEAN_AND_MEAN
#include "screenChangeDetector.h"

void screenChangeDetectorInitialize(ScreenChangeDetector *detector,
                                    BYTE channelThreshold,
                                    double changePercentageThreshold) {
  if (detector == NULL)
    return;
  ZeroMemory(detector, sizeof(*detector));
  detector->channelThreshold = channelThreshold;
  detector->changePercentageThreshold =
      changePercentageThreshold < 0.0
          ? 0.0
          : (changePercentageThreshold > 100.0 ? 100.0
                                               : changePercentageThreshold);
}

void screenChangeDetectorReset(ScreenChangeDetector *detector) {
  if (detector == NULL)
    return;
  captureImageFree(&detector->previousFrame);
  detector->hasBaseline = FALSE;
}

void screenChangeDetectorDestroy(ScreenChangeDetector *detector) {
  if (detector != NULL)
    screenChangeDetectorReset(detector);
}

BOOL screenChangeDetectorUpdate(ScreenChangeDetector *detector,
                                const CaptureImage *currentFrame, BOOL *changed,
                                ImageComparisonResult *comparison) {
  if (detector == NULL || currentFrame == NULL ||
      currentFrame->pixels == NULL || changed == NULL || comparison == NULL)
    return FALSE;

  ZeroMemory(comparison, sizeof(*comparison));
  *changed = FALSE;
  if (!detector->hasBaseline ||
      detector->previousFrame.width != currentFrame->width ||
      detector->previousFrame.height != currentFrame->height) {
    CaptureImage replacement = {0};
    if (!captureImageClone(currentFrame, &replacement))
      return FALSE;
    captureImageFree(&detector->previousFrame);
    detector->previousFrame = replacement;
    detector->hasBaseline = TRUE;
    return TRUE;
  }

  if (!imageComparisonCompare(&detector->previousFrame, currentFrame, NULL,
                              detector->channelThreshold, comparison, NULL))
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