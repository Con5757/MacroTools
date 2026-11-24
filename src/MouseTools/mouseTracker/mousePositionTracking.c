#include "mousePositionTracking.h"

BOOL mousePositionTracking(POINT *position) {
  if (position == NULL) {
    return FALSE;
  }

  return GetCursorPos(position);
}
