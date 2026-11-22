#pragma once
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

// Rectangle convention: left, right x top, bottom

// shared types
typedef struct {
  double x;
  double y;
} CTPoint;
typedef struct {
  double left;
  double top;
  double right;
  double bottom;
} CTRect;
typedef enum {
  CT_STATUS_OK = 0,
  CT_STATUS_INVALID_ARGUMENT,
  CT_STATUS_BUFFER_TOO_SMALL,
  CT_STATUS_OUT_OF_RANGE,
  CT_STATUS_IO_ERROR,
  CT_STATUS_INVALID_FORMAT,
  CT_STATUS_DEGENERATE_CALIBRATION
} CTStatus;

#define CT_PROFILE_NAME_CAPACITY 64

static inline bool ctIsFinite(double value) { return isfinite(value) != 0; }
static inline bool ctPointIsFinite(CTPoint point) {
  return ctIsFinite(point.x) && ctIsFinite(point.y);
}
static inline bool ctRectIsValid(CTRect rect) {
  return ctIsFinite(rect.left) && ctIsFinite(rect.top) &&
         ctIsFinite(rect.right) && ctIsFinite(rect.bottom) &&
         rect.right > rect.left && rect.bottom > rect.top;
}
static inline double ctRectWidth(CTRect rect) { return rect.right - rect.left; }
static inline double ctRectHeight(CTRect rect) {
  return rect.bottom - rect.top;
}
static inline bool ctRectContainsPoint(CTRect rect, CTPoint point) {
  return ctRectIsValid(rect) && ctPointIsFinite(point) &&
         point.x >= rect.left && point.x < rect.right && point.y >= rect.top &&
         point.y < rect.bottom;
}
static inline double ctRectLastX(CTRect rect) {
  return rect.right - 1.0;
} // last coord in the halfO pixel coord domain
static inline double ctRectLastY(CTRect rect) { return rect.bottom - 1.0; }