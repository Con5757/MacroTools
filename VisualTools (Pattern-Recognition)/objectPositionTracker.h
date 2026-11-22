#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const VTImage *templateImage;
  VTPoint lastPosition;
  int searchRadius;
  int step;
  double minimumSimilarity;
  BOOL initialized;
} VTObjectTracker;
typedef struct {
  VTPoint position;
  VTRect bounds;
  double similarity;
  BOOL found;
} VTObjectTrackResult;

VTStatus vtObjectTrackerInitialize(VTObjectTracker *tracker,
                                   const VTImage *templateImage,
                                   int searchRadius, int step,
                                   double minimumSimilarity);
VTStatus vtObjectTrackerUpdate(VTObjectTracker *tracker, const VTImage *frame,
                               VTObjectTrackResult *result);

#ifdef __cplusplus
}
#endif