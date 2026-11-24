#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const VTImage *templateImage;
  double minimumSimilarity;
  void *userData;
} VTTargetDefinition;
typedef struct {
  size_t targetIndex;
  VTPoint position;
  VTRect bounds;
  double similarity;
  void *userData;
} VTTargetMatch;

VTStatus vtDetectMultipleTargets(const VTImage *image, VTRect region,
                                 const VTTargetDefinition *targets,
                                 size_t targetCount, int step,
                                 VTTargetMatch *matches, size_t capacity,
                                 size_t *matchCount);


#ifdef __cplusplus
}
#endif