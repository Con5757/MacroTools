#define WIN32_LEAN_AND_MEAN
#include "imageComparisonTool.h"

#include <string.h>

BOOL imageComparisonCompare(const CaptureImage *first,
                            const CaptureImage *second, const RECT *region,
                            BYTE channelThreshold,
                            ImageComparisonResult *result,
                            CaptureImage *differenceImage) {
  if (first == NULL || second == NULL || result == NULL ||
      first->pixels == NULL || second->pixels == NULL ||
      first->width != second->width || first->height != second->height)
    return FALSE;

  RECT area = {0, 0, (LONG)first->width, (LONG)first->height};
  if (region != NULL)
    area = *region;
  if (area.left < 0 || area.top < 0 || area.right > (LONG)first->width ||
      area.bottom > (LONG)first->height || area.right <= area.left ||
      area.bottom <= area.top)
    return FALSE;

  if (differenceImage != NULL &&
      !captureImageCreate(differenceImage, first->width, first->height))
    return FALSE;

  ZeroMemory(result, sizeof(*result));
  unsigned long long absoluteTotal = 0;
  for (LONG y = area.top; y < area.bottom; ++y) {
    const BYTE *a =
        first->pixels + (size_t)y * first->stride + (size_t)area.left * 4u;
    const BYTE *b =
        second->pixels + (size_t)y * second->stride + (size_t)area.left * 4u;
    BYTE *d = differenceImage ? differenceImage->pixels +
                                    (size_t)y * differenceImage->stride +
                                    (size_t)area.left * 4u
                              : NULL;
    for (LONG x = area.left; x < area.right; ++x, a += 4, b += 4) {
      BYTE maxDiff = 0;
      BOOL different = FALSE;
      for (int channel = 0; channel < 3; ++channel) {
        int delta = (int)a[channel] - (int)b[channel];
        BYTE magnitude = (BYTE)(delta < 0 ? -delta : delta);
        absoluteTotal += magnitude;
        if (magnitude > maxDiff)
          maxDiff = magnitude;
        if (magnitude > channelThreshold)
          different = TRUE;
        if (d != NULL) {
          unsigned int amplified = (unsigned int)magnitude * 4u;
          d[channel] = (BYTE)(amplified > 255u ? 255u : amplified);
        }
      }
      if (d != NULL) {
        d[3] = 255;
        d += 4;
      }
      if (maxDiff > result->maximumChannelDifference)
        result->maximumChannelDifference = maxDiff;
      result->comparedPixels++;
      if (different)
        result->differentPixels++;
    }
  }

  if (result->comparedPixels != 0) {
    result->changedPercentage = 100.0 * (double)result->differentPixels /
                                (double)result->comparedPixels;
    result->meanAbsoluteError =
        (double)absoluteTotal / ((double)result->comparedPixels * 3.0);
  }
  return TRUE;
}