#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

VTStatus vtDetectEdges(const VTImage *source, unsigned int threshold,
                       VTImage *edgeMask, size_t *edgePixels);

#ifdef __cplusplus
}
#endif