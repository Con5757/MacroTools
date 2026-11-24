#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t changedPixels;
  double changedRatio;
  VTRect bounds;
  BOOL motionDetected;
} VTMotionResult;

VTStatus vtDetectMotion(const VTImage *previous, const VTImage *current,
                        uint8_t pixelDifferenceThreshold,
                        double motionRatioThreshold, VTMotionResult *result,
                        VTImage *differenceMask);

#ifdef __cplusplus
}
#endif