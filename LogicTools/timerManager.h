#pragma once
#include "logicToolsCommon.h"

typedef struct {
  uint32_t id;
  uint64_t startMs;
  uint64_t durationMs;
  bool active;
  bool occupied;
} LATimer;
typedef struct {
  LATimer timers[LA_MAX_ITEMS];
} LATimerManager;

void laTimerManagerInitialize(LATimerManager *manager);
bool laTimerStart(LATimerManager *manager, uint32_t id, uint64_t nowMs,
                  uint64_t durationMs);
bool laTimerStop(LATimerManager *manager, uint32_t id);
bool laTimerIsActive(const LATimerManager *manager, uint32_t id);
bool laTimerIsExpired(const LATimerManager *manager, uint32_t id,
                      uint64_t nowMs);
bool laTimerElapsed(const LATimerManager *manager, uint32_t id, uint64_t nowMs,
                    uint64_t *elapsedMs);