#pragma once
#include "logicToolsCommon.h"

typedef struct {
  uint32_t actionId;
  uint64_t lastUseMs;
  uint64_t cooldownMs;
  bool occupied;
  bool hasUsed;
} LACooldownEntry;
typedef struct {
  LACooldownEntry entries[LA_MAX_ITEMS];
} LACooldownManager;

void laCooldownManagerInitialize(LACooldownManager *manager);
bool laCooldownConfigure(LACooldownManager *manager, uint32_t actionId,
                         uint64_t cooldownMs);
bool laCooldownReady(const LACooldownManager *manager, uint32_t actionId,
                     uint64_t nowMs);
bool laCooldownTryAcquire(LACooldownManager *manager, uint32_t actionId,
                          uint64_t nowMs);