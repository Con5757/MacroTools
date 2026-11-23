#pragma once
#include "logicToolsCommon.h"

typedef struct {
  uint32_t id;
  uint64_t startMs;
  uint64_t durationMs;
  bool active;
} LATimeout;
typedef struct {
  LATimeout timeouts[LA_MAX_ITEMS];
} LATimeoutManager;

void laTimeoutManagerInitialize(LATimeoutManager *manager);
bool laTimeoutStart(LATimeoutManager *manager, uint32_t id, uint64_t nowMs,
                    uint64_t durationMs);
bool laTimeoutCancel(LATimeoutManager *manager, uint32_t id);
bool laTimeoutExpired(const LATimeoutManager *manager, uint32_t id,
                      uint64_t nowMs);
uint64_t laTimeoutRemaining(const LATimeoutManager *manager, uint32_t id,
                            uint64_t nowMs, bool *found);