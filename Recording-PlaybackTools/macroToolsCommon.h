#pragma once
#ifndef MACRO_TOOLS_COMMON_H
#define MACRO_TOOLS_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MACRO_NAME_CAPACITY 64
#define MACRO_TEXT_CAPACITY 96
#define MACRO_NO_TARGET UINT32_MAX

typedef enum {
  MACRO_STEP_KEY_DOWN = 1,
  MACRO_STEP_KEY_UP,
  MACRO_STEP_MOUSE_MOVE,
  MACRO_STEP_MOUSE_BUTTON_DOWN,
  MACRO_STEP_MOUSE_BUTTON_UP,
  MACRO_STEP_MOUSE_SCROLL,
  MACRO_STEP_WAIT,
  MACRO_STEP_SET_VARIABLE,
  MACRO_STEP_CONDITION,
  MACRO_STEP_JUMP,
  MACRO_STEP_CHECKPOINT
} MacroStepType;
typedef struct {
  uint64_t id;
  uint32_t type;
  uint32_t flags;
  uint64_t delay_ms;
  int32_t x;
  int32_t y;
  int32_t code;
  int32_t value;
  double number;
  uint32_t target_step;
  uint32_t alternate_step;
  char text[MACRO_TEXT_CAPACITY];
} MacroStep;
typedef struct {
  char name[MACRO_NAME_CAPACITY];
  MacroStep *steps;
  size_t count;
  size_t capacity;
  uint64_t next_id;
} MacroSequence;
typedef enum {
  MACRO_CONDITION_EQUAL = 0,
  MACRO_CONDITION_NOT_EQUAL,
  MACRO_CONDITION_GREATER,
  MACRO_CONDITION_GREATER_EQUAL,
  MACRO_CONDITION_LESS,
  MACRO_CONDITION_LESS_EQUAL,
  MACRO_CONDITION_TRUTHY,
  MACRO_CONDITION_FALSEY
} MacroConditionOperator;
typedef struct {
  char name[MACRO_NAME_CAPACITY];
  double value;
} MacroVariable;
typedef struct {
  MacroVariable *items;
  size_t count;
  size_t capacity;
} MacroVariableStore;
typedef bool (*MacroDispatchFunction)(const MacroStep *step, void *context);
typedef bool (*MacroConditionFunction)(const MacroStep *step, void *context);
typedef struct {
  const MacroSequence *sequence;
  size_t step_index;
  uint64_t due_at_ms;
  uint64_t pause_started_ms;
  uint64_t loops_total; // 0 = forever -> 1 = one iteration
  uint64_t loops_completed;
  double speed;
  bool active;
  bool paused;
  bool stop_requested;
  bool single_step;
  MacroDispatchFunction dispatch;
  void *dispatch_context;
  MacroConditionFunction condition;
  void *condition_context;
} MacroPlaybackState;
typedef enum {
  MACRO_PLAYER_ERROR = -1,
  MACRO_PLAYER_IDLE = 0,
  MACRO_PLAYER_PROGRESS = 1,
  MACRO_PLAYER_FINISHED = 2
} MacroPlayerResult;

void macroStepInitialize(MacroStep *step, MacroStepType type);
void macroSequenceInitialize(MacroSequence *sequence, const char *name);
void macroSequenceDestroy(MacroSequence *sequence);
bool macroSequenceReserve(MacroSequence *sequence, size_t capacity);
bool macroSequenceAppend(MacroSequence *sequence, const MacroStep *step);
bool macroSequenceInsert(MacroSequence *sequence, size_t index,
                         const MacroStep *step);
bool macroSequenceRemove(MacroSequence *sequence, size_t index);
bool macroSequenceClone(MacroSequence *destination,
                        const MacroSequence *source);
MacroStep *macroSequenceGet(MacroSequence *sequence, size_t index);
const MacroStep *macroSequenceGetConst(const MacroSequence *sequence,
                                       size_t index);

void macroVariableStoreInitialize(MacroVariableStore *store);
void macroVariableStoreDestroy(MacroVariableStore *store);
bool macroVariableStoreSet(MacroVariableStore *store, const char *name,
                           double value);
bool macroVariableStoreGet(const MacroVariableStore *store, const char *name,
                           double *value);
bool macroVariableStoreRemove(MacroVariableStore *store, const char *name);

bool macroConditionEvaluate(double left, MacroConditionOperator op,
                            double right, bool *result);
bool macroConditionEvaluateStep(const MacroStep *step,
                                const MacroVariableStore *variables,
                                bool *result);

bool macroPlaybackPause(MacroPlaybackState *state, uint64_t now_ms);
bool macroPlaybackResume(MacroPlaybackState *state, uint64_t now_ms);
void macroPlaybackRequestStop(MacroPlaybackState *state);
bool macroPlaybackSetSpeed(MacroPlaybackState *state, double speed);

#endif