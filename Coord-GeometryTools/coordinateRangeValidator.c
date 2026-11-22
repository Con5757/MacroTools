#include "coordinateRangeValidator.h"

CTStatus ctValidateCoordinateRange(CTPoint position, CTRect bounds,
                                   bool *isWithinRange) {
  if (isWithinRange == NULL || !ctPointIsFinite(position) ||
      !ctRectIsValid(bounds)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  *isWithinRange = ctRectContainsPoint(bounds, position);
  return CT_STATUS_OK;
}