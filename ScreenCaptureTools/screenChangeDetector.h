#pragma once
#include "common/captureImage.h"
#include "imageComparisonTool.h"

typedef struct {
  CaptureImage previousFrame;
  BYTE channelThreshold;
  double changePercentageThreshold;
  BOOL hasBaseline;
} ScreenChangeDetector;

void screenChangeDetectorInitialize(ScreenChangeDetector *detector,
                                    BYTE channelThreshold,
                                    double changePercentageThreshold);
void screenChangeDetectorReset(ScreenChangeDetector *detector);
void screenChangeDetectorDestroy(ScreenChangeDetector *detector);
BOOL screenChangeDetectorUpdate(ScreenChangeDetector *detector,
                                const CaptureImage *currentFrame, BOOL *changed,
                                ImageComparisonResult *comparison);