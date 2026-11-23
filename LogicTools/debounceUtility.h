#pragma once
#include "logicToolsCommon.h"

typedef struct {
  uint64_t intervalMs;
  uint64_t lastAcceptedMs;
  bool initialized;
} LADebouncer;

void laDebounceInitialize(LADebouncer *debouncer, uint64_t intervalMs);
// events inside debounce are rejected
bool laDebounceAccept(LADebouncer *debouncer, uint64_t nowMs);
void laDebounceReset(LADebouncer *debouncer);