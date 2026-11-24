#include "sequenceGenerator.h"
#include <math.h>

void laSequenceReset(LASequenceGenerator *s, double start) {
  if (!s)
    return;
  s->nextValue = start;
  s->remaining = 0;
  s->active = false;
}
bool laSequenceInitialize(LASequenceGenerator *s, double start, double step,
                          uint64_t count) {
  if (!s || !isfinite(start) || !isfinite(step) || count == 0)
    return false;
  s->nextValue = start;
  s->step = step;
  s->remaining = count;
  s->active = true;
  return true;
}
bool laSequenceNext(LASequenceGenerator *s, double *out) {
  if (!s || !out || !s->active || s->remaining == 0)
    return false;
  *out = s->nextValue;
  s->remaining--;
  if (s->remaining == 0)
    s->active = false;
  else {
    s->nextValue += s->step;
    if (!isfinite(s->nextValue))
      s->active = false;
  }
  return true;
}