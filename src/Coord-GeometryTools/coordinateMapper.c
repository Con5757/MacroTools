#include "coordinateMapper.h"

CTStatus ctMapCoordinate(CTPoint position, CTRect sourceBounds,
                         CTRect targetBounds, CTPoint *mappedPosition) {
  if (mappedPosition == NULL || !ctPointIsFinite(position) ||
      !ctRectIsValid(sourceBounds) || !ctRectIsValid(targetBounds)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  const double sourceXSpan =
      ctRectWidth(sourceBounds) > 1.0 ? ctRectWidth(sourceBounds) - 1.0 : 0.0;
  const double sourceYSpan =
      ctRectHeight(sourceBounds) > 1.0 ? ctRectHeight(sourceBounds) - 1.0 : 0.0;
  const double targetXSpan =
      ctRectWidth(targetBounds) > 1.0 ? ctRectWidth(targetBounds) - 1.0 : 0.0;
  const double targetYSpan =
      ctRectHeight(targetBounds) > 1.0 ? ctRectHeight(targetBounds) - 1.0 : 0.0;

  const double normalizedX =
      sourceXSpan > 0.0 ? (position.x - sourceBounds.left) / sourceXSpan : 0.0;
  const double normalizedY =
      sourceYSpan > 0.0 ? (position.y - sourceBounds.top) / sourceYSpan : 0.0;

  mappedPosition->x = targetBounds.left + normalizedX * targetXSpan;
  mappedPosition->y = targetBounds.top + normalizedY * targetYSpan;

  if (!ctPointIsFinite(*mappedPosition)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  return CT_STATUS_OK;
}