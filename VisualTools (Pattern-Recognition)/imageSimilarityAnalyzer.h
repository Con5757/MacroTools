#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  double meanAbsoluteError;
  double rootMeanSquareError;
  double similarity;
  size_t comparedChannels;
} VTImageSimilarity;

VTStatus vtImageCompare(const VTImage *a, const VTImage *b,
                        VTImageSimilarity *result);

#ifdef __cplusplus
}
#endif