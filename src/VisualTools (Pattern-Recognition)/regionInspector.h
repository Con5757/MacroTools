#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  VTRect region;
  VTRgb average;
  VTRgb minimum;
  VTRgb maximum;
  double averageBrightness;
  size_t pixelCount;
} VTRegionInspection;

VTStatus vtRegionInspect(const VTImage *image, VTRect region,
                         VTRegionInspection *inspection);

#ifdef __cplusplus
}
#endif