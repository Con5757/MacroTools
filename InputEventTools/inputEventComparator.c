#include "inputEventComparator.h"
#include <stdlib.h>
#include <string.h>

static ULONGLONG difference64(ULONGLONG a, ULONGLONG b) {
  return a >= b ? a - b : b - a;
}
static ULONGLONG differenceLong(LONG a, LONG b) {
  int64_t delta = (int64_t)a - (int64_t)b;
  return (ULONGLONG)(delta < 0 ? -delta : delta);
}
BOOL inputEventComparatorCompare(const InputEvent *left, size_t leftCount,
                                 const InputEvent *right, size_t rightCount,
                                 ULONGLONG tolerance, LONG coordinateTolerance,
                                 InputEventComparison *result) {
  if (result == NULL || (leftCount && left == NULL) ||
      (rightCount && right == NULL) || coordinateTolerance < 0)
    return FALSE;
  memset(result, 0, sizeof(*result));
  size_t common = leftCount < rightCount ? leftCount : rightCount;
  double timingSum = 0.0;
  for (size_t i = 0; i < common; ++i) {
    const InputEvent *a = &left[i], *b = &right[i];
    ULONGLONG dt = difference64(a->timestampMs, b->timestampMs);
    if (dt > result->maxTimestampDifferenceMs)
      result->maxTimestampDifferenceMs = dt;
    timingSum += (double)dt;
    BOOL timingMatch = dt <= tolerance;
    BOOL payloadMatch =
        a->type == b->type && a->virtualKey == b->virtualKey &&
        a->scanCode == b->scanCode && a->mouseButton == b->mouseButton &&
        a->wheelDelta == b->wheelDelta &&
        differenceLong(a->x, b->x) <= (ULONGLONG)coordinateTolerance &&
        differenceLong(a->y, b->y) <= (ULONGLONG)coordinateTolerance;
    if (!timingMatch)
      ++result->timingMismatches;
    if (!payloadMatch)
      ++result->payloadMismatches;
    if (timingMatch && payloadMatch)
      ++result->matching;
    else
      ++result->mismatched;
  }
  result->compared = common;
  result->leftOnly = leftCount > common ? leftCount - common : 0;
  result->rightOnly = rightCount > common ? rightCount - common : 0;
  result->meanTimestampDifferenceMs = common ? timingSum / (double)common : 0.0;
  result->equivalent = result->mismatched == 0 && result->leftOnly == 0 &&
                       result->rightOnly == 0;
  return TRUE;
}