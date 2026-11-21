#pragma once
#include "common/captureImage.h"

typedef struct {
  size_t comparedPixels;
  size_t differentPixels;
  double changedPercentage;
  double meanAbsoluteError;
  BYTE maximumChannelDifference;
} ImageComparisonResult;

BOOL imageComparisonCompare(const CaptureImage *first,
                            const CaptureImage *second, const RECT *region,
                            BYTE channelThreshold,
                            ImageComparisonResult *result,
                            CaptureImage *differenceImage);