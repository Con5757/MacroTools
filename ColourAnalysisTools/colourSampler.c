#include "colourSampler.h"

#include <stdint.h>

BOOL colourSamplerSampleRegion(int x, int y, int width, int height,
                               PixelColourSample *sample) {
  if (sample == NULL)
    return FALSE;

  PixelImage image;
  if (!pixelImageCapture(x, y, width, height, &image))
    return FALSE;

  uint64_t sumR = 0, sumG = 0, sumB = 0;
  size_t total = (size_t)image.width * (size_t)image.height;
  size_t i;
  PixelColor minimum = image.pixels[0];
  PixelColor maximum = image.pixels[0];

  for (i = 0; i < total; ++i) {
    PixelColor c = image.pixels[i];
    sumR += c.r;
    sumG += c.g;
    sumB += c.b;

    if (c.r < minimum.r)
      minimum.r = c.r;
    if (c.g < minimum.g)
      minimum.g = c.g;
    if (c.b < minimum.b)
      minimum.b = c.b;
    if (c.r > maximum.r)
      maximum.r = c.r;
    if (c.g > maximum.g)
      maximum.g = c.g;
    if (c.b > maximum.b)
      maximum.b = c.b;
  }

  sample->average.r = (BYTE)(sumR / total);
  sample->average.g = (BYTE)(sumG / total);
  sample->average.b = (BYTE)(sumB / total);
  sample->minimum = minimum;
  sample->maximum = maximum;
  sample->center =
      image.pixels[(size_t)(image.height / 2) * (size_t)image.width +
                   (size_t)(image.width / 2)];
  sample->pixelCount = total;

  pixelImageDestroy(&image);
  return TRUE;
}