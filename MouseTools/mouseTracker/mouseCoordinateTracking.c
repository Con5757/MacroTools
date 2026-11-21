#include "mouseCoordinateTracking.h"

BOOL mouseCoordinateTracking(const POINT *desktopPosition,
                             const RECT *monitorBounds,
                             POINT *monitorPosition) {

  if (desktopPosition == NULL || monitorBounds == NULL ||
      monitorPosition == NULL) {
    return FALSE;
  }

  monitorPosition->x = desktopPosition->x - monitorBounds->left;
  monitorPosition->y = desktopPosition->y - monitorBounds->top;

  return TRUE;
}
