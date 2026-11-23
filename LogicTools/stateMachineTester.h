#pragma once
#include "logicToolsCommon.h"

typedef struct {
  uint32_t fromState;
  uint32_t toState;
  LAConditionCallback condition;
  void *context;
  bool occupied;
} LAStateTransition;
typedef struct {
  uint32_t currentState;
  LAStateTransition transitions[LA_MAX_ITEMS];
  size_t transitionCount;
  uint64_t transitionCountTotal;
} LAStateMachine;

void laStateMachineInitialize(LAStateMachine *machine, uint32_t initialState);
bool laStateMachineAddTransition(LAStateMachine *machine, uint32_t fromState,
                                 uint32_t toState,
                                 LAConditionCallback condition, void *context);
bool laStateMachineStep(LAStateMachine *machine); // returns true if changed
uint32_t laStateMachineGetState(const LAStateMachine *machine);
void laStateMachineReset(LAStateMachine *machine, uint32_t initialState);