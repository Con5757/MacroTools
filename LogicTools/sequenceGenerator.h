#pragma once
#include "logicToolsCommon.h"

typedef struct {
  double nextValue;
  double step;
  uint64_t remaining;
  bool active;
} LASequenceGenerator;

bool laSequenceInitialize(LASequenceGenerator *sequence, double start,
                          double step, uint64_t count);
// count must be greater than 0
bool laSequenceNext(LASequenceGenerator *sequence, double *value);
void laSequenceReset(LASequenceGenerator *sequence, double start);