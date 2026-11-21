#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

typedef enum {
  MOUSE_BOUNDARY_NO_EVENT = 0,
  MOUSE_BOUNDARY_ENTERED,
  MOUSE_BOUNDARY_EXITED
} MouseBoundaryEventType;

typedef enum {
  MOUSE_BOUNDARY_EDGE_UNKNOWN = 0,
  MOUSE_BOUNDARY_EDGE_LEFT,
  MOUSE_BOUNDARY_EDGE_TOP,
  MOUSE_BOUNDARY_EDGE_RIGHT,
  MOUSE_BOUNDARY_EDGE_BOTTOM
} MouseBoundaryEdge;

typedef struct {
  RECT bounds;
  POINT previousPosition;
  BOOL wasInside;
  BOOL initialized;
} MouseBoundaryDetector;

typedef struct {
  MouseBoundaryEventType type;
  MouseBoundaryEdge edge;
  POINT position;
  BOOL isInside;
} MouseBoundaryEvent;

BOOL mouseBoundaryDetectorInitialize(MouseBoundaryDetector *detector,
                                     RECT bounds);
BOOL mouseBoundaryDetectorUpdate(MouseBoundaryDetector *detector,
                                 MouseBoundaryEvent *event);
void mouseBoundaryDetectorSetBounds(MouseBoundaryDetector *detector,
                                    RECT bounds);