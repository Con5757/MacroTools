#include "inventoryGridInspector.h"
#include <math.h>

bool inventoryGridInspectorInspect(const GTImage *frame, GTRect gridRegion,
                                   int rows, int columns,
                                   GTColor emptyCellColor,
                                   double occupancyTolerance,
                                   GTInventoryCell *cells,
                                   size_t cellCapacity) {
  if (rows <= 0 || columns <= 0 || cells == NULL ||
      (size_t)rows > SIZE_MAX / (size_t)columns ||
      cellCapacity < (size_t)rows * (size_t)columns ||
      occupancyTolerance < 0.0 || occupancyTolerance > 765.0 ||
      !gtRectIsValidForImage(frame, gridRegion) || gridRegion.width < columns ||
      gridRegion.height < rows)
    return false;

  for (int row = 0; row < rows; ++row) {
    int top = gridRegion.y + (int)((int64_t)row * gridRegion.height / rows);
    int bottom =
        gridRegion.y + (int)((int64_t)(row + 1) * gridRegion.height / rows);
    for (int col = 0; col < columns; ++col) {
      int left =
          gridRegion.x + (int)((int64_t)col * gridRegion.width / columns);
      int right =
          gridRegion.x + (int)((int64_t)(col + 1) * gridRegion.width / columns);
      size_t index = (size_t)row * (size_t)columns + (size_t)col;
      GTInventoryCell *cell = &cells[index];
      cell->row = row;
      cell->column = col;
      cell->bounds = (GTRect){left, top, right - left, bottom - top};
      GTImageRegionStats stats;
      if (!gtImageMeasureRegion(frame, cell->bounds, &stats))
        return false;
      cell->meanColor = stats.mean;
      cell->brightnessVariance = stats.brightnessVariance;
      cell->differenceFromEmpty =
          gtColorManhattanDistance(stats.mean, emptyCellColor);
      cell->occupied = cell->differenceFromEmpty > occupancyTolerance;
    }
  }
  return true;
}