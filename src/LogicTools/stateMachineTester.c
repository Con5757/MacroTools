#include "stateMachineTester.h"
#include <string.h>

void laStateMachineInitialize(LAStateMachine *machine, uint32_t initialState) {
  if (machine == NULL)
    return;
  memset(machine, 0, sizeof(*machine));
  machine->currentState = initialState;
}
bool laStateMachineAddTransition(LAStateMachine *machine, uint32_t fromState,
                                 uint32_t toState,
                                 LAConditionCallback condition, void *context) {
  if (machine == NULL || machine->transitionCount >= LA_MAX_ITEMS ||
      fromState == toState)
    return false;
  LAStateTransition *t = &machine->transitions[machine->transitionCount++];
  t->fromState = fromState;
  t->toState = toState;
  t->condition = condition;
  t->context = context;
  t->occupied = true;
  return true;
}
bool laStateMachineStep(LAStateMachine *machine) {
  if (machine == NULL)
    return false;
  for (size_t i = 0; i < machine->transitionCount; ++i) {
    LAStateTransition *t = &machine->transitions[i];
    if (t->occupied && t->fromState == machine->currentState &&
        (t->condition == NULL || t->condition(t->context))) {
      machine->currentState = t->toState;
      machine->transitionCountTotal++;
      return true;
    }
  }
  return false;
}
uint32_t laStateMachineGetState(const LAStateMachine *machine) {
  return machine != NULL ? machine->currentState : LA_INVALID_ID;
}
void laStateMachineReset(LAStateMachine *machine, uint32_t initialState) {
  if (machine == NULL)
    return;
  machine->currentState = initialState;
  machine->transitionCountTotal = 0;
}