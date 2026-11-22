#include "macroBranchingEngine.h"

bool macroBranchingResolve(const MacroSequence *sequence, size_t current,
                           const MacroVariableStore *vars, size_t *next) {
  if (!sequence || !next || current >= sequence->count)
    return false;
  const MacroStep *step = &sequence->steps[current];
  if (step->type == MACRO_STEP_JUMP) {
    if (step->target_step >= sequence->count)
      return false;
    *next = step->target_step;
    return true;
  }
  if (step->type == MACRO_STEP_CONDITION) {
    bool pass;
    if (!macroConditionEvaluateStep(step, vars, &pass))
      return false;
    uint32_t target = pass ? step->target_step : step->alternate_step;
    if (target == MACRO_NO_TARGET) {
      *next = current + 1;
      return true;
    }
    if (target >= sequence->count)
      return false;
    *next = target;
    return true;
  }
  *next = current + 1;
  return true;
}