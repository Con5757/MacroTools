#pragma once

#include "ocrResult.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef VTStatus (*VTOcrEngine)(const VTImage *image, VTRect region,
                                char *utf8Output, size_t outputCapacity,
                                void *userData);
typedef VTStatus (*VTOcrStructuredEngine)(const VTImage *image, VTRect region,
                                          VTOcrResult *result, void *userData);

VTStatus vtTextRecognize(const VTImage *image, VTRect region,
                         VTOcrEngine engine, void *userData, char *utf8Output,
                         size_t outputCapacity);
VTStatus vtTextRecognizeStructured(const VTImage *image, VTRect region,
                                   VTOcrStructuredEngine engine, void *userData,
                                   VTOcrResult *result);

#ifdef __cplusplus
}
#endif