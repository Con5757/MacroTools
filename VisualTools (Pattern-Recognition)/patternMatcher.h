#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int x;
  int y;
  VTRgb color;
  uint8_t tolerance;
} VTPatternPoint;
typedef struct {
  VTPoint anchor;
  double confidence;
} VTPatternMatch;

VTStatus vtPatternFind(const VTImage *image, VTRect searchRegion,
                       const VTPatternPoint *points, size_t pointCount,
                       VTPatternMatch *match);
                       
#ifdef __cplusplus
}
#endif