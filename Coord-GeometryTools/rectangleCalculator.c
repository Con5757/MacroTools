#include "rectangleCalculator.h"

CTStatus ctCreateRectangle(double left, double top, double width, double height,
                           CTRect *rectangle) {
  if (rectangle == NULL || !ctIsFinite(left) || !ctIsFinite(top) ||
      !ctIsFinite(width) || !ctIsFinite(height) || width <= 0.0 ||
      height <= 0.0) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  CTRect result = {left, top, left + width, top + height};
  if (!ctRectIsValid(result)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  *rectangle = result;
  return CT_STATUS_OK;
}
CTStatus ctCalculateRectangle(CTRect rectangle, CTRectangleMetrics *metrics) {
  if (metrics == NULL || !ctRectIsValid(rectangle)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  const double width = ctRectWidth(rectangle);
  const double height = ctRectHeight(rectangle);
  const double area = width * height;

  if (!ctIsFinite(area)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  metrics->bounds = rectangle;
  metrics->width = width;
  metrics->height = height;
  metrics->area = area;
  metrics->center.x = rectangle.left + (width - 1.0) / 2.0;
  metrics->center.y = rectangle.top + (height - 1.0) / 2.0;

  if (!ctPointIsFinite(metrics->center)) {
    return CT_STATUS_OUT_OF_RANGE;
  }

  return CT_STATUS_OK;
}