#include "imageSimilarityAnalyzer.h"
#include <math.h>

VTStatus vtImageCompare(const VTImage *a, const VTImage *b,
                        VTImageSimilarity *out) {
  if (!vtImageIsValid(a) || !vtImageIsValid(b) || out == NULL ||
      a->width != b->width || a->height != b->height)
    return VT_STATUS_INVALID_ARGUMENT;
  uint64_t absSum = 0;
  long double squareSum = 0.0L;
  size_t channels = (size_t)a->width * (size_t)a->height * 3u;
  for (int y = 0; y < a->height; ++y) {
    const uint8_t *pa = a->pixels + (size_t)y * (size_t)a->stride;
    const uint8_t *pb = b->pixels + (size_t)y * (size_t)b->stride;
    for (int x = 0; x < a->width * 3; ++x) {
      int d = (int)pa[x] - (int)pb[x];
      unsigned int ad = (unsigned int)(d < 0 ? -d : d);
      absSum += ad;
      squareSum += (long double)d * (long double)d;
    }
  }
  double mae = (double)absSum / (double)channels;
  double rmse = sqrt((double)(squareSum / (long double)channels));
  out->meanAbsoluteError = mae;
  out->rootMeanSquareError = rmse;
  out->similarity = 1.0 - mae / 255.0;
  out->comparedChannels = channels;
  return VT_STATUS_OK;
}