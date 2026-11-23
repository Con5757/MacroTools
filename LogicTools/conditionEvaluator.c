#include "conditionEvaluator.h"
#include <math.h>

bool laConditionCompare(double left, LACompareOperator operation, double right,
                        double epsilon) {
  if (!isfinite(left) || !isfinite(right) || !isfinite(epsilon))
    return false;
  if (epsilon < 0.0)
    epsilon = -epsilon;
  switch (operation) {
  case LA_COMPARE_EQUAL:
    return fabs(left - right) <= epsilon;
  case LA_COMPARE_NOT_EQUAL:
    return fabs(left - right) > epsilon;
  case LA_COMPARE_LESS:
    return left < right;
  case LA_COMPARE_LESS_EQUAL:
    return left <= right;
  case LA_COMPARE_GREATER:
    return left > right;
  case LA_COMPARE_GREATER_EQUAL:
    return left >= right;
  default:
    return false;
  }
}
bool laConditionAnd(bool left, bool right) { return left && right; }
bool laConditionOr(bool left, bool right) { return left || right; }
bool laConditionNot(bool value) { return !value; }