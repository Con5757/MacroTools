#pragma once
#include "macroToolsCommon.h"

typedef struct {
  uint32_t position_jitter_x;
  uint32_t position_jitter_y;
  uint32_t delay_jitter_ms;
  uint64_t seed;
} MacroRandomizerConfig;

void macroRandomizerInitialize(MacroRandomizerConfig *config, uint64_t seed);
bool macroRandomizerApply(MacroRandomizerConfig *config,
                          MacroSequence *sequence);