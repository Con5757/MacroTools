#pragma once
#include "coordinateToolsCommon.h"

// calculates origin
CTStatus ctCalculateCoordinateOffset(CTPoint origin, CTPoint destination,
                                     CTPoint *offset);
// gives an offset
CTStatus ctApplyCoordinateOffset(CTPoint position, CTPoint offset,
                                 CTPoint *result);