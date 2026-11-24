#include "macroPlayer.h"
#include <math.h>
#include <string.h>

static uint64_t scaledDelay(uint64_t delay, double speed) {
    if (delay == 0) return 0;
    long double scaled = (long double)delay / (long double)speed;
    if (scaled >= (long double)UINT64_MAX) return UINT64_MAX;
    uint64_t value = (uint64_t)ceill(scaled);
    return value == 0 ? 1 : value;
}
void macroPlayerInitialize(MacroPlaybackState *state) {
    if (state == NULL) return;
    memset(state, 0, sizeof(*state));
    state->speed = 1.0;
    state->loops_total = 1;
}
bool macroPlayerStart(MacroPlaybackState *state, const MacroSequence *sequence, uint64_t now_ms,
                      MacroDispatchFunction dispatch, void *dispatch_context) {
    if (state == NULL || sequence == NULL || sequence->count == 0 || sequence->steps == NULL || state->active) return false;
    MacroConditionFunction condition = state->condition;
    void *condition_context = state->condition_context;
    uint64_t loops_total = state->loops_total;
    double speed = state->speed;
    if (!isfinite(speed) || speed < 0.05 || speed > 20.0) speed = 1.0;
    memset(state, 0, sizeof(*state));
    state->condition = condition;
    state->condition_context = condition_context;
    state->loops_total = loops_total;
    state->speed = speed;
    state->sequence = sequence;
    state->dispatch = dispatch;
    state->dispatch_context = dispatch_context;
    state->step_index = 0;
    uint64_t first_delay = scaledDelay(sequence->steps[0].delay_ms, speed);
    state->due_at_ms = UINT64_MAX - now_ms < first_delay ? UINT64_MAX : now_ms + first_delay;
    state->active = true;
    return true;
}
void macroPlayerSetConditionHandler(MacroPlaybackState *state, MacroConditionFunction condition, void *context) {
    if (state == NULL) return;
    state->condition = condition;
    state->condition_context = context;
}
MacroPlayerResult macroPlayerUpdate(MacroPlaybackState *state, uint64_t now_ms, size_t max_steps_per_update,
                                    size_t *steps_processed) {
    if (steps_processed != NULL) *steps_processed = 0;
    if (state == NULL) return MACRO_PLAYER_ERROR;
    if (!state->active) return MACRO_PLAYER_IDLE;
    if (state->stop_requested) {
        state->active = false;
        state->paused = false;
        return MACRO_PLAYER_FINISHED;
    }
    if (state->paused || state->sequence == NULL) return MACRO_PLAYER_IDLE;
    if (max_steps_per_update == 0) max_steps_per_update = 64;
    size_t processed = 0;
    while (state->active && !state->paused && now_ms >= state->due_at_ms && processed < max_steps_per_update) {
        if (state->stop_requested) {
            state->active = false;
            break;
        }
        if (state->step_index >= state->sequence->count) {
            state->loops_completed++;
            if (state->loops_total != 0 && state->loops_completed >= state->loops_total) {
                state->active = false;
                break;
            }
            state->step_index = 0;
            uint64_t d = scaledDelay(state->sequence->steps[0].delay_ms, state->speed);
            state->due_at_ms = UINT64_MAX - state->due_at_ms < d ? UINT64_MAX : state->due_at_ms + d;
            continue;
        }
        const MacroStep *step = &state->sequence->steps[state->step_index];
        size_t next_index = state->step_index + 1;
        bool internal_control = false;
        if (step->type == MACRO_STEP_JUMP) {
            if (step->target_step >= state->sequence->count) { state->active = false; return MACRO_PLAYER_ERROR; }
            next_index = step->target_step;
            internal_control = true;
        } else if (step->type == MACRO_STEP_CONDITION) {
            if (state->condition == NULL) { state->active = false; return MACRO_PLAYER_ERROR; }
            bool passed = state->condition(step, state->condition_context);
            uint32_t target = passed ? step->target_step : step->alternate_step;
            if (target != MACRO_NO_TARGET) {
                if (target >= state->sequence->count) { state->active = false; return MACRO_PLAYER_ERROR; }
                next_index = target;
            }
            internal_control = true;
        } else if (step->type != MACRO_STEP_WAIT) {
            if (state->dispatch != NULL && !state->dispatch(step, state->dispatch_context)) {
                state->active = false;
                return MACRO_PLAYER_ERROR;
            }
        }
        (void)internal_control;
        processed++;
        state->step_index = next_index;
        if (state->single_step) {
            state->single_step = false;
            state->paused = true;
            state->pause_started_ms = now_ms;
            break;
        }
        if (state->step_index < state->sequence->count) {
            uint64_t d = scaledDelay(state->sequence->steps[state->step_index].delay_ms, state->speed);
            state->due_at_ms = UINT64_MAX - state->due_at_ms < d ? UINT64_MAX : state->due_at_ms + d;
        }
    }
    if (steps_processed != NULL) *steps_processed = processed;
    if (!state->active) return MACRO_PLAYER_FINISHED;
    return processed > 0 ? MACRO_PLAYER_PROGRESS : MACRO_PLAYER_IDLE;
}