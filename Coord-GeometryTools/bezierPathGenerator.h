#pragma once
#include "coordinateToolsCommon.h"

// generates a cubic bezier path with a minimum with sampleCount -> minimum of
// 2, first and final are 0->3
CTStatus
ctGenerateBezierPath(CTPoint p0, CTPoint p1, CTPoint p2, CTPoint p3,
                     size_t sampleCount, CTPoint *points, size_t capacity,
                     size_t *requiredCount); // let pass = NULL and capacity = 0
                                             // to query the expected count