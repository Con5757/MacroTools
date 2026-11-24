#pragma once
#include "coordinateToolsCommon.h"

// generates a straight pass (with segments) with both endpoints
CTStatus
ctInterpolatePath(CTPoint start, CTPoint end, size_t segments, CTPoint *points,
                  size_t capacity,
                  size_t *requiredCount); // to query counts pass points=NULL
                                          // and capacity=0 like usual