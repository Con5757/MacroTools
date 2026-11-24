#include "gameToolsCommon.h"
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

bool gtImageIsValid(const GTImage *image) {
  if (image == NULL || image->pixels == NULL || image->width <= 0 ||
      image->height <= 0) {
    return false;
  }

  if ((size_t)image->width > SIZE_MAX / 3u) {
    return false;
  }

  size_t minimumStride = (size_t)image->width * 3u;
  return image->stride >= minimumStride &&
         (size_t)image->height <= SIZE_MAX / image->stride;
}
bool gtRectIsValidForImage(const GTImage *image, GTRect region) {
  if (!gtImageIsValid(image) || region.x < 0 || region.y < 0 ||
      region.width <= 0 || region.height <= 0) {
    return false;
  }

  return region.x <= image->width - region.width &&
         region.y <= image->height - region.height;
}
bool gtImageGetPixel(const GTImage *image, int x, int y, GTColor *color) {
  if (color == NULL || !gtImageIsValid(image) || x < 0 || y < 0 ||
      x >= image->width || y >= image->height) {
    return false;
  }

  const uint8_t *pixel =
      image->pixels + (size_t)y * image->stride + (size_t)x * 3u;
  color->r = pixel[0];
  color->g = pixel[1];
  color->b = pixel[2];
  return true;
}
double gtColorManhattanDistance(GTColor a, GTColor b) {
  int dr = (int)a.r - (int)b.r;
  int dg = (int)a.g - (int)b.g;
  int db = (int)a.b - (int)b.b;
  if (dr < 0)
    dr = -dr;
  if (dg < 0)
    dg = -dg;
  if (db < 0)
    db = -db;
  return (double)(dr + dg + db);
}
bool gtImageMeasureRegion(const GTImage *image, GTRect region,
                          GTImageRegionStats *stats) {
  if (stats == NULL || !gtRectIsValidForImage(image, region)) {
    return false;
  }

  uint64_t sums[3] = {0, 0, 0};
  uint64_t brightnessSum = 0;
  uint64_t brightnessSquaredSum = 0;
  uint8_t minimum[3] = {255, 255, 255};
  uint8_t maximum[3] = {0, 0, 0};
  size_t count = (size_t)region.width * (size_t)region.height;

  for (int y = region.y; y < region.y + region.height; ++y) {
    const uint8_t *row =
        image->pixels + (size_t)y * image->stride + (size_t)region.x * 3u;
    for (int x = 0; x < region.width; ++x) {
      uint8_t r = row[(size_t)x * 3u];
      uint8_t g = row[(size_t)x * 3u + 1u];
      uint8_t b = row[(size_t)x * 3u + 2u];
      uint16_t brightness =
          (uint16_t)((299u * r + 587u * g + 114u * b) / 1000u);

      sums[0] += r;
      sums[1] += g;
      sums[2] += b;
      brightnessSum += brightness;
      brightnessSquaredSum += (uint64_t)brightness * brightness;

      if (r < minimum[0])
        minimum[0] = r;
      if (g < minimum[1])
        minimum[1] = g;
      if (b < minimum[2])
        minimum[2] = b;
      if (r > maximum[0])
        maximum[0] = r;
      if (g > maximum[1])
        maximum[1] = g;
      if (b > maximum[2])
        maximum[2] = b;
    }
  }

  double meanBrightness = (double)brightnessSum / (double)count;
  double variance = (double)brightnessSquaredSum / (double)count -
                    meanBrightness * meanBrightness;
  if (variance < 0.0)
    variance = 0.0;

  stats->mean.r = (uint8_t)(sums[0] / count);
  stats->mean.g = (uint8_t)(sums[1] / count);
  stats->mean.b = (uint8_t)(sums[2] / count);
  stats->minimum.r = minimum[0];
  stats->minimum.g = minimum[1];
  stats->minimum.b = minimum[2];
  stats->maximum.r = maximum[0];
  stats->maximum.g = maximum[1];
  stats->maximum.b = maximum[2];
  stats->meanBrightness = meanBrightness;
  stats->brightnessVariance = variance;
  stats->pixelCount = count;
  return true;
}
bool gtImageFindTemplate(const GTImage *image, const GTImage *templ,
                         double maximumMeanDifference, GTImageMatch *match) {
  if (match == NULL || !gtImageIsValid(image) || !gtImageIsValid(templ) ||
      templ->width > image->width || templ->height > image->height ||
      !isfinite(maximumMeanDifference) || maximumMeanDifference < 0.0 ||
      maximumMeanDifference > 255.0) {
    return false;
  }

  match->found = false;
  match->bounds = (GTRect){0, 0, 0, 0};
  match->meanChannelDifference = 255.0;

  double best = 256.0;
  size_t totalChannels = (size_t)templ->width * (size_t)templ->height * 3u;

  for (int y = 0; y <= image->height - templ->height; ++y) {
    for (int x = 0; x <= image->width - templ->width; ++x) {
      uint64_t difference = 0;
      bool rejected = false;

      for (int ty = 0; ty < templ->height && !rejected; ++ty) {
        const uint8_t *source =
            image->pixels + (size_t)(y + ty) * image->stride + (size_t)x * 3u;
        const uint8_t *target = templ->pixels + (size_t)ty * templ->stride;

        for (int tx = 0; tx < templ->width * 3; ++tx) {
          int delta = (int)source[tx] - (int)target[tx];
          difference += (uint64_t)(delta < 0 ? -delta : delta);
          double partialMean = (double)difference / (double)totalChannels;
          if (partialMean > best || partialMean > maximumMeanDifference) {
            rejected = true;
            break;
          }
        }
      }

      if (!rejected) {
        double score = (double)difference / (double)totalChannels;
        if (score < best) {
          best = score;
          match->found = true;
          match->bounds = (GTRect){x, y, templ->width, templ->height};
          match->meanChannelDifference = score;
        }
      }
    }
  }

  return true;
}
bool gtImageCopyRegion(const GTImage *image, GTRect region, uint8_t **pixels,
                       size_t *stride) {
  if (pixels == NULL || stride == NULL ||
      !gtRectIsValidForImage(image, region)) {
    return false;
  }

  size_t rowBytes = (size_t)region.width * 3u;
  if ((size_t)region.height > SIZE_MAX / rowBytes) {
    return false;
  }

  uint8_t *copy = (uint8_t *)malloc(rowBytes * (size_t)region.height);
  if (copy == NULL) {
    return false;
  }

  for (int y = 0; y < region.height; ++y) {
    const uint8_t *source = image->pixels +
                            (size_t)(region.y + y) * image->stride +
                            (size_t)region.x * 3u;
    memcpy(copy + (size_t)y * rowBytes, source, rowBytes);
  }

  *pixels = copy;
  *stride = rowBytes;
  return true;
}