#include "colourFinder.h"

#include <math.h>
#include <stdlib.h>

static void colourFinderClearResults(PixelPointArray *matches) {
  if (matches == NULL)
    return;
  free(matches->items);
  matches->items = NULL;
  matches->count = 0;
  matches->capacity = 0;
}

BOOL colourFinderSearch(int x, int y, int width, int height, PixelColor target,
                        double tolerance, PixelPointArray *matches) {
  if (matches == NULL || !isfinite(tolerance) || tolerance < 0.0)
    return FALSE;
  colourFinderClearResults(matches);

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  double toleranceSquared = tolerance * tolerance;
  size_t total = (size_t)image.width * (size_t)image.height;
  size_t i;
  BOOL success = TRUE;

  for (i = 0; i < total; ++i) {
    PixelColor c = image.pixels[i];
    double dr = (double)c.r - target.r;
    double dg = (double)c.g - target.g;
    double db = (double)c.b - target.b;
    if (dr * dr + dg * dg + db * db <= toleranceSquared) {
      PixelPoint point;
      point.x = image.x + (int)(i % (size_t)image.width);
      point.y = image.y + (int)(i / (size_t)image.width);
      point.color = c;
      if (!pixelPointArrayAppend(matches, point)) {
        success = FALSE;
        break;
      }
    }
  }

  pixelImageDestroy(&image);
  if (!success)
    colourFinderClearResults(matches);
  return success;
}

BOOL colourFinderSearchVirtualDesktop(PixelColor target, double tolerance,
                                      PixelPointArray *matches) {
  int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  return colourFinderSearch(x, y, width, height, target, tolerance, matches);
}