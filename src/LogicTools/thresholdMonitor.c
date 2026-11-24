#include "thresholdMonitor.h"
#include <math.h>

bool laThresholdInitialize(LAThresholdMonitor *m, double t,
                           LAThresholdMode mode) {
  if (!m || !isfinite(t) || mode < LA_THRESHOLD_ABOVE ||
      mode > LA_THRESHOLD_CROSS_ANY)
    return false;
  m->threshold = t;
  m->mode = mode;
  m->initialized = false;
  m->previousAbove = false;
  return true;
}
bool laThresholdUpdate(LAThresholdMonitor *m, double v, bool *out) {
  if (!m || !out || !isfinite(v))
    return false;
  bool above = v >= m->threshold;
  *out = false;
  if (m->initialized) {
    bool crossed = above != m->previousAbove;
    if (m->mode == LA_THRESHOLD_ABOVE)
      *out = crossed && above;
    else if (m->mode == LA_THRESHOLD_BELOW)
      *out = crossed && !above;
    else
      *out = crossed;
  }
  m->initialized = true;
  m->previousAbove = above;
  return true;
}