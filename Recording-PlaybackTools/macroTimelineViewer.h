#pragma once
#include "macroToolsCommon.h"

typedef struct {
  size_t step_index;
  uint64_t step_id;
  uint64_t at_ms;
  uint32_t step_type;
} MacroTimelineEntry;

// returns the required entry count -> should write almost all entries when the
// output isnt null
size_t macroTimelineBuild(const MacroSequence *sequence,
                          MacroTimelineEntry *output, size_t capacity);