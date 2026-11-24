#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t activePixels;
  double occupancy;
  VTRect activityBounds;
  BOOL occupied;
} VTRegionOccupancy;

VTStatus vtRegionMeasureOccupancy(const VTImage *current,
                                  const VTImage *baseline, VTRect region,
                                  uint8_t differenceThreshold,
                                  double minimumOccupancy,
                                  VTRegionOccupancy *result);

#ifdef __cplusplus
}
#endif