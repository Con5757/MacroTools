#include "gridGenerator.h"
#include <stdint.h>

static CTStatus ctGridAxisCount(double span, double spacing, size_t *count) {
  if (count == NULL || !ctIsFinite(span) || !ctIsFinite(spacing) ||
      span < 0.0 || spacing <= 0.0) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  const long double intervals =
      floorl((long double)span / (long double)spacing);
  if (intervals < 0.0L || intervals >= (long double)SIZE_MAX) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  *count = (size_t)intervals + 1;
  return CT_STATUS_OK;
}
CTStatus ctGenerateCoordinateGrid(CTRect bounds, double spacingX,
                                  double spacingY, CTPoint *points,
                                  size_t capacity, size_t *requiredCount) {
  if (requiredCount == NULL || !ctRectIsValid(bounds) ||
      !ctIsFinite(spacingX) || !ctIsFinite(spacingY) || spacingX <= 0.0 ||
      spacingY <= 0.0 || (points == NULL && capacity != 0)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  size_t columns = 0;
  size_t rows = 0;
  const double xSpan =
      ctRectWidth(bounds) > 1.0 ? ctRectWidth(bounds) - 1.0 : 0.0;
  const double ySpan =
      ctRectHeight(bounds) > 1.0 ? ctRectHeight(bounds) - 1.0 : 0.0;
  CTStatus status = ctGridAxisCount(xSpan, spacingX, &columns);
  if (status != CT_STATUS_OK) {
    return status;
  }
  status = ctGridAxisCount(ySpan, spacingY, &rows);
  if (status != CT_STATUS_OK) {
    return status;
  }

  if (columns != 0 && rows > SIZE_MAX / columns) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  const size_t needed = columns * rows;
  *requiredCount = needed;

  if (points == NULL || capacity < needed) {
    return CT_STATUS_BUFFER_TOO_SMALL;
  }

  size_t index = 0;
  for (size_t row = 0; row < rows; ++row) {
    const double y = bounds.top + (double)row * spacingY;
    for (size_t column = 0; column < columns; ++column) {
      const double x = bounds.left + (double)column * spacingX;
      if (!ctIsFinite(x) || !ctIsFinite(y) || x > ctRectLastX(bounds) ||
          y > ctRectLastY(bounds)) {
        return CT_STATUS_OUT_OF_RANGE;
      }
      points[index++] = (CTPoint){x, y};
    }
  }

  return CT_STATUS_OK;
}