#include "macroEditor.h"
#include <stdlib.h>
#include <string.h>

bool macroEditorInsertStep(MacroSequence *sequence, size_t index,
                           const MacroStep *step) {
  return macroSequenceInsert(sequence, index, step);
}
bool macroEditorDeleteStep(MacroSequence *sequence, size_t index) {
  return macroSequenceRemove(sequence, index);
}
bool macroEditorMoveStep(MacroSequence *sequence, size_t from_index,
                         size_t to_index) {
  if (sequence == NULL || from_index >= sequence->count ||
      to_index >= sequence->count)
    return false;
  if (from_index == to_index)
    return true;
  MacroStep value = sequence->steps[from_index];
  if (from_index < to_index)
    memmove(&sequence->steps[from_index], &sequence->steps[from_index + 1],
            (to_index - from_index) * sizeof(MacroStep));
  else
    memmove(&sequence->steps[to_index + 1], &sequence->steps[to_index],
            (from_index - to_index) * sizeof(MacroStep));
  sequence->steps[to_index] = value;
  return true;
}
bool macroEditorDuplicateStep(MacroSequence *sequence, size_t index,
                              size_t insert_at) {
  const MacroStep *step = macroSequenceGetConst(sequence, index);
  if (step == NULL)
    return false;
  MacroStep copy = *step;
  copy.id = 0;
  return macroSequenceInsert(sequence, insert_at, &copy);
}