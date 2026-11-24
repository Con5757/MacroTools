#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  int row;
  int column;
  GTRect bounds;
  GTColor meanColor;
  double brightnessVariance;
  double differenceFromEmpty;
  bool occupied;
} GTInventoryCell;

bool inventoryGridInspectorInspect(const GTImage *frame, GTRect gridRegion,
                                   int rows, int columns,
                                   GTColor emptyCellColor,
                                   double occupancyTolerance,
                                   GTInventoryCell *cells, size_t cellCapacity);