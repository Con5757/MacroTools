#pragma once
#include "coordinateToolsCommon.h"

// normalize pixel coords to 0-1 accros the f/l pixel centers
CTStatus ctNormalizeCoordinate(CTPoint position, CTRect bounds,
                               CTPoint *normalized);
// convert normalized 0-1 coords back to the pixel coord domain
CTStatus ctDenormalizeCoordinate(CTPoint normalized, CTRect bounds,
                                 CTPoint *position);