#include "coordinateNormalizer.h"

CTStatus ctNormalizeCoordinate(CTPoint position, CTRect bounds,
                               CTPoint *normalized) {
  if (normalized == NULL || !ctPointIsFinite(position) ||
      !ctRectIsValid(bounds)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  const double xSpan =
      ctRectWidth(bounds) > 1.0 ? ctRectWidth(bounds) - 1.0 : 0.0;
  const double ySpan =
      ctRectHeight(bounds) > 1.0 ? ctRectHeight(bounds) - 1.0 : 0.0;

  normalized->x = xSpan > 0.0 ? (position.x - bounds.left) / xSpan : 0.0;
  normalized->y = ySpan > 0.0 ? (position.y - bounds.top) / ySpan : 0.0;

  return ctPointIsFinite(*normalized) ? CT_STATUS_OK : CT_STATUS_OUT_OF_RANGE;
}
CTStatus ctDenormalizeCoordinate(CTPoint normalized, CTRect bounds,
                                 CTPoint *position) {
  if (position == NULL || !ctPointIsFinite(normalized) ||
      !ctRectIsValid(bounds)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  const double xSpan =
      ctRectWidth(bounds) > 1.0 ? ctRectWidth(bounds) - 1.0 : 0.0;
  const double ySpan =
      ctRectHeight(bounds) > 1.0 ? ctRectHeight(bounds) - 1.0 : 0.0;

  position->x = bounds.left + normalized.x * xSpan;
  position->y = bounds.top + normalized.y * ySpan;

  if (!ctPointIsFinite(*position)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  return CT_STATUS_OK;
}