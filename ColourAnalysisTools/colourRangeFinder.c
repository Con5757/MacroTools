#include "colourRangeFinder.h"

#include <math.h>
#include <stdlib.h>

static void clearMatches(PixelPointArray *matches) {
  free(matches->items);
  matches->items = NULL;
  matches->count = 0;
  matches->capacity = 0;
}

static BOOL rgbInRange(PixelColor c, PixelRGBRange r) {
  return c.r >= r.minimum.r && c.r <= r.maximum.r && c.g >= r.minimum.g &&
         c.g <= r.maximum.g && c.b >= r.minimum.b && c.b <= r.maximum.b;
}

BOOL colourRangeFinderSearchRGB(int x, int y, int width, int height,
                                PixelRGBRange range, PixelPointArray *matches) {
  if (matches == NULL || range.minimum.r > range.maximum.r ||
      range.minimum.g > range.maximum.g || range.minimum.b > range.maximum.b) {
    return FALSE;
  }
  clearMatches(matches);

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  size_t total = (size_t)image.width * (size_t)image.height;
  size_t i;
  BOOL success = TRUE;
  for (i = 0; i < total; ++i) {
    if (rgbInRange(image.pixels[i], range)) {
      PixelPoint point = {image.x + (int)(i % (size_t)image.width),
                          image.y + (int)(i / (size_t)image.width),
                          image.pixels[i]};
      if (!pixelPointArrayAppend(matches, point)) {
        success = FALSE;
        break;
      }
    }
  }

  pixelImageDestroy(&image);
  if (!success)
    clearMatches(matches);
  return success;
}

static BOOL hueInRange(double hue, double minimum, double maximum) {
  // if a min is greater than max -> range must cross 0
  if (minimum <= maximum)
    return hue >= minimum && hue <= maximum;
  return hue >= minimum || hue <= maximum;
}

BOOL colourRangeFinderSearchHSV(int x, int y, int width, int height,
                                PixelHSVRange range, PixelPointArray *matches) {
  if (matches == NULL || !isfinite(range.minimumHue) ||
      !isfinite(range.maximumHue) || !isfinite(range.minimumSaturation) ||
      !isfinite(range.maximumSaturation) || !isfinite(range.minimumValue) ||
      !isfinite(range.maximumValue) || range.minimumHue < 0.0 ||
      range.minimumHue > 360.0 || range.maximumHue < 0.0 ||
      range.maximumHue > 360.0 || range.minimumSaturation < 0.0 ||
      range.minimumSaturation > 1.0 || range.maximumSaturation < 0.0 ||
      range.maximumSaturation > 1.0 || range.minimumValue < 0.0 ||
      range.minimumValue > 1.0 || range.maximumValue < 0.0 ||
      range.maximumValue > 1.0 ||
      range.minimumSaturation > range.maximumSaturation ||
      range.minimumValue > range.maximumValue) {
    return FALSE;
  }
  clearMatches(matches);

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  size_t total = (size_t)image.width * (size_t)image.height;
  size_t i;
  BOOL success = TRUE;
  for (i = 0; i < total; ++i) {
    double h, s, v;
    pixelColorToHSV(image.pixels[i], &h, &s, &v);
    if (hueInRange(h, range.minimumHue, range.maximumHue) &&
        s >= range.minimumSaturation && s <= range.maximumSaturation &&
        v >= range.minimumValue && v <= range.maximumValue) {
      PixelPoint point = {image.x + (int)(i % (size_t)image.width),
                          image.y + (int)(i / (size_t)image.width),
                          image.pixels[i]};
      if (!pixelPointArrayAppend(matches, point)) {
        success = FALSE;
        break;
      }
    }
  }

  pixelImageDestroy(&image);
  if (!success)
    clearMatches(matches);
  return success;
}