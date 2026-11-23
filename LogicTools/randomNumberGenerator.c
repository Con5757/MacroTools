#include "randomNumberGenerator.h"
#include <math.h>

void laRandomSeed(LARandomGenerator *g, uint64_t seed) {
  if (g)
    g->state = seed ? seed : UINT64_C(0x9E3779B97F4A7C15);
}
uint64_t laRandomNextU64(LARandomGenerator *g) {
  if (!g)
    return 0;
  uint64_t x = g->state;
  if (x == 0)
    x = UINT64_C(0x9E3779B97F4A7C15);
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  g->state = x;
  return x * UINT64_C(2685821657736338717);
}
uint32_t laRandomRangeU32(LARandomGenerator *g, uint32_t min, uint32_t max) {
  if (!g || max < min)
    return min;
  uint64_t span = (uint64_t)max - (uint64_t)min + 1u;
  uint64_t threshold = (uint64_t)(-span) % span;
  uint64_t r;
  do {
    r = laRandomNextU64(g);
  } while (r < threshold);
  return min + (uint32_t)(r % span);
}
double laRandomRangeDouble(LARandomGenerator *g, double min, double max) {
  if (!g || !isfinite(min) || !isfinite(max) || max < min)
    return min;
  uint64_t r = laRandomNextU64(g) >> 11;
  double unit = (double)r * (1.0 / 9007199254740992.0);
  return min + (max - min) * unit;
}