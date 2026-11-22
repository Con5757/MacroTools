#include "macroConditionManager.h"

bool macroConditionManagerEvaluate(const MacroStep *step,
                                   const MacroVariableStore *variables,
                                   bool *result) {
  if (step == NULL || step->type != MACRO_STEP_CONDITION)
    return false;
  return macroConditionEvaluateStep(step, variables, result);
}