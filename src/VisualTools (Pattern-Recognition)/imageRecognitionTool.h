#pragma once
#include "templateMatcher.h"
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const char *name;
  const VTImage *templateImage;
  double minimumSimilarity;
} VTRecognitionClass;
typedef struct {
  size_t classIndex;
  const char *name;
  VTTemplateMatch match;
} VTRecognitionResult;

VTStatus vtImageRecognize(const VTImage *image, VTRect region,
                          const VTRecognitionClass *classes, size_t classCount,
                          int step, VTRecognitionResult *result);

#ifdef __cplusplus
}
#endif