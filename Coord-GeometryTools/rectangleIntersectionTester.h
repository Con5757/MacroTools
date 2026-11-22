#pragma once
#include "coordinateToolsCommon.h"

// tests if two halfO rects overlap -> if true should return a pos overlap
CTStatus ctTestRectangleIntersection(CTRect first, CTRect second,
                                     bool *intersects, CTRect *intersection); // no intersection is set to rect (0, 0)