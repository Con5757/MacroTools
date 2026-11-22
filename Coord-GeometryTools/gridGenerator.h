#pragma once
#include "coordinateToolsCommon.h"

// generates a rmjr grid of points inside the provided bounds -> left/top
// spacing must be pos and the last coord is never right-1/bottom-1
CTStatus ctGenerateCoordinateGrid(
    CTRect bounds, double spacingX, double spacingY, CTPoint *points,
    size_t capacity,
    size_t
        *requiredCount); // set points=NULL and capacity to query requiredCount