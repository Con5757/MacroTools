#include "macroStepInspector.h"
#include <string.h>

bool macroStepInspectorGet(const MacroSequence *sequence, size_t index,
                           MacroStep *output) {
  const MacroStep *step = macroSequenceGetConst(sequence, index);
  if (step == NULL || output == NULL)
    return false;
  *output = *step;
  return true;
}