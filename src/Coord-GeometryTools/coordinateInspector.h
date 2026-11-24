#pragma once
#include "coordinateToolsCommon.h"

typedef struct {
  CTPoint position;
  CTPoint relativePosition;
  CTPoint normalizedPosition;
  bool withinBounds;
} CTCoordinateInspection;

// to inspect a coord relative to to a half open screen rect
CTStatus ctInspectCoordinate(CTPoint position, CTRect bounds,
                             CTCoordinateInspection *inspection);