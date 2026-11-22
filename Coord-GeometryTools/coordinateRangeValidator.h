#pragma once
#include "coordinateToolsCommon.h"

// returns `CT_STATUS_OK` and writes the contain result -> unsure if itll print
// if its outside, will be guarenteed inside a later patch
CTStatus ctValidateCoordinateRange(CTPoint position, CTRect bounds,
                                   bool *isWithinRange);