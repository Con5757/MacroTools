#include "macroCheckpointManager.h"
#include <math.h>

bool macroCheckpointManagerSave(const MacroPlaybackState *s, uint64_t now,
                                MacroCheckpoint *c) {
  if (!s || !c || !s->active)
    return false;
  c->step_index = s->step_index;
  c->loops_completed = s->loops_completed;
  c->remaining_delay_ms = s->due_at_ms > now ? s->due_at_ms - now : 0;
  c->speed = s->speed;
  c->valid = true;
  return true;
}
bool macroCheckpointManagerRestore(MacroPlaybackState *s, uint64_t now,
                                   const MacroCheckpoint *c) {
  if (!s || !c || !c->valid || !s->sequence ||
      c->step_index >= s->sequence->count || !isfinite(c->speed) ||
      c->speed < 0.05 || c->speed > 20.0)
    return false;
  s->step_index = c->step_index;
  s->loops_completed = c->loops_completed;
  s->speed = c->speed;
  s->due_at_ms = UINT64_MAX - now < c->remaining_delay_ms
                     ? UINT64_MAX
                     : now + c->remaining_delay_ms;
  s->active = true;
  s->paused = false;
  s->stop_requested = false;
  return true;
}