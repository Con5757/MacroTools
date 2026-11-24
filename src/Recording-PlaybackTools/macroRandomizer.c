#include "macroRandomizer.h"
#include <limits.h>

void macroRandomizerInitialize(MacroRandomizerConfig *c, uint64_t seed) {
  if (!c)
    return;
  c->position_jitter_x = 0;
  c->position_jitter_y = 0;
  c->delay_jitter_ms = 0;
  c->seed = seed ? seed : UINT64_C(0x9E3779B97F4A7C15);
}
static uint32_t nextRandom(MacroRandomizerConfig *c) {
  uint64_t x = c->seed;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  c->seed = x;
  return (uint32_t)((x * UINT64_C(2685821657736338717)) >> 32);
}
static int64_t signedJitter(MacroRandomizerConfig *c, uint32_t range) {
  if (range == 0)
    return 0;
  uint64_t span = (uint64_t)range * 2u + 1u;
  return (int64_t)((uint64_t)nextRandom(c) % span) - (int64_t)range;
}
static int32_t clampI32(int64_t v) {
  return v > INT32_MAX ? INT32_MAX : (v < INT32_MIN ? INT32_MIN : (int32_t)v);
}
bool macroRandomizerApply(MacroRandomizerConfig *c, MacroSequence *s) {
  if (!c || !s)
    return false;
  for (size_t i = 0; i < s->count; i++) {
    MacroStep *p = &s->steps[i];
    if (p->type == MACRO_STEP_MOUSE_MOVE) {
      p->x = clampI32((int64_t)p->x + signedJitter(c, c->position_jitter_x));
      p->y = clampI32((int64_t)p->y + signedJitter(c, c->position_jitter_y));
    }
    if (c->delay_jitter_ms) {
      int64_t d = signedJitter(c, c->delay_jitter_ms);
      if (d < 0 && (uint64_t)(-d) > p->delay_ms)
        p->delay_ms = 0;
      else if (d < 0)
        p->delay_ms -= (uint64_t)(-d);
      else if (UINT64_MAX - p->delay_ms < (uint64_t)d)
        p->delay_ms = UINT64_MAX;
      else
        p->delay_ms += (uint64_t)d;
    }
  }
  return true;
}