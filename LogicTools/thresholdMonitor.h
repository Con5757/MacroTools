#pragma once
#include "logicToolsCommon.h"

typedef enum {
  LA_THRESHOLD_ABOVE,
  LA_THRESHOLD_BELOW,
  LA_THRESHOLD_CROSS_ANY
} LAThresholdMode;
typedef struct {
  double threshold;
  LAThresholdMode mode;
  bool initialized;
  bool previousAbove;
} LAThresholdMonitor;

bool laThresholdInitialize(LAThresholdMonitor *monitor, double threshold,
                           LAThresholdMode mode);
bool laThresholdUpdate(LAThresholdMonitor *monitor, double value,
                       bool *triggered);