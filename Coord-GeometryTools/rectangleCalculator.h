#pragma once
#include "coordinateToolsCommon.h"

typedef struct {
  CTRect bounds;
  double width;
  double height;
  double area;
  CTPoint center;
} CTRectangleMetrics;

// create a rect from its topL coord and +dimens
CTStatus ctCreateRectangle(double left, double top, double width, double height,
                           CTRect *rectangle);
// calculate dimen&pixel area&center of a halfO rect
CTStatus ctCalculateRectangle(CTRect rectangle, CTRectangleMetrics *metrics);