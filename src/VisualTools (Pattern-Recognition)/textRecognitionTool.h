#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef VTStatus (*VTOcrEngine)(const VTImage *image, VTRect region,
                                char *utf8Output, size_t outputCapacity,
                                void *userData);
VTStatus vtTextRecognize(const VTImage *image, VTRect region,
                         VTOcrEngine engine, void *userData, char *utf8Output,
                         size_t outputCapacity);

#ifdef __cplusplus
}
#endif