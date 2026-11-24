#include "coordinateOffsetCalculator.h"

CTStatus ctCalculateCoordinateOffset(CTPoint origin, CTPoint destination,
                                     CTPoint *offset) {
  if (offset == NULL || !ctPointIsFinite(origin) ||
      !ctPointIsFinite(destination)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  offset->x = destination.x - origin.x;
  offset->y = destination.y - origin.y;

  return ctPointIsFinite(*offset) ? CT_STATUS_OK : CT_STATUS_OUT_OF_RANGE;
}
CTStatus ctApplyCoordinateOffset(CTPoint position, CTPoint offset,
                                 CTPoint *result) {
  if (result == NULL || !ctPointIsFinite(position) ||
      !ctPointIsFinite(offset)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  result->x = position.x + offset.x;
  result->y = position.y + offset.y;

  return ctPointIsFinite(*result) ? CT_STATUS_OK : CT_STATUS_OUT_OF_RANGE;
}