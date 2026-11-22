#include "macroTimelineViewer.h"
#include <stdint.h>

size_t macroTimelineBuild(const MacroSequence *sequence,
                          MacroTimelineEntry *output, size_t capacity) {
  if (sequence == NULL || (sequence->count > 0 && sequence->steps == NULL))
    return 0;
  uint64_t elapsed = 0;
  for (size_t i = 0; i < sequence->count; ++i) {
    const MacroStep *step = &sequence->steps[i];
    elapsed = UINT64_MAX - elapsed < step->delay_ms ? UINT64_MAX
                                                    : elapsed + step->delay_ms;
    if (output != NULL && i < capacity) {
      output[i].step_index = i;
      output[i].step_id = step->id;
      output[i].at_ms = elapsed;
      output[i].step_type = step->type;
    }
  }
  return sequence->count;
}