#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  VTPoint position;
  double similarity;
  VTRect bounds;
} VTTemplateMatch;

VTStatus vtTemplateMatch(const VTImage *image, const VTImage *templateImage,
                         VTRect searchRegion, int step, VTTemplateMatch *match);

#ifdef __cplusplus
}
#endif