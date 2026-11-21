#pragma once
#include "common/captureImage.h"
#include "imageComparisonTool.h"

typedef struct {
  CaptureImage previousFrame;
  RECT region;
  BYTE channelThreshold;
  double changePercentageThreshold;
  BOOL initialized;
} RegionChangeDetector;

BOOL regionChangeDetectorInitialize(RegionChangeDetector *detector,
                                    const RECT *imageRelativeRegion,
                                    BYTE channelThreshold,
                                    double changePercentageThreshold);
void regionChangeDetectorReset(RegionChangeDetector *detector);
void regionChangeDetectorDestroy(RegionChangeDetector *detector);
BOOL regionChangeDetectorUpdate(RegionChangeDetector *detector,
                                const CaptureImage *currentFrame, BOOL *changed,
                                ImageComparisonResult *comparison);