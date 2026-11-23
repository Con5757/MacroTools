#pragma once
#include "logicToolsCommon.h"

typedef struct {
  uint64_t state;
} LARandomGenerator;

void laRandomSeed(LARandomGenerator *generator, uint64_t seed);
uint64_t laRandomNextU64(LARandomGenerator *generator);
uint32_t laRandomRangeU32(LARandomGenerator *generator, uint32_t minimum,
                          uint32_t maximum);
double laRandomRangeDouble(LARandomGenerator *generator, double minimum,
                           double maximum);