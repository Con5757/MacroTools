#pragma once
#include "coordinateToolsCommon.h"

// keeps the relative pixel pos -> maps a coord between those two pixels
CTStatus ctMapCoordinate(CTPoint position, CTRect sourceBounds,
                         CTRect targetBounds, CTPoint *mappedPosition);