#include "recognitionAccuracyTester.h"
#include <stdlib.h>

VTStatus vtRecognitionMeasureAccuracy(const VTRect *pred, size_t predCount,
                                      const VTRect *truth, size_t truthCount,
                                      double threshold, VTAccuracyReport *out) {
  if ((predCount && pred == NULL) || (truthCount && truth == NULL) ||
      out == NULL || threshold < 0.0 || threshold > 1.0)
    return VT_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < predCount; ++i)
    if (!vtRectIsValid(pred[i]))
      return VT_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < truthCount; ++i)
    if (!vtRectIsValid(truth[i]))
      return VT_STATUS_INVALID_ARGUMENT;
  uint8_t *used = (uint8_t *)calloc(truthCount ? truthCount : 1u, 1u);
  if (!used)
    return VT_STATUS_OUT_OF_MEMORY;
  size_t tp = 0, fp = 0;
  for (size_t i = 0; i < predCount; ++i) {
    double best = threshold;
    size_t bestIndex = truthCount;
    for (size_t j = 0; j < truthCount; ++j) {
      if (used[j] || !vtRectIsValid(truth[j]))
        continue;
      double iou = vtRectIntersectionOverUnion(pred[i], truth[j]);
      if (iou > 0.0 && iou >= best) {
        best = iou;
        bestIndex = j;
      }
    }
    if (bestIndex < truthCount) {
      used[bestIndex] = 1;
      ++tp;
    } else
      ++fp;
  }
  size_t fn = truthCount - tp;
  free(used);
  double precision = tp + fp ? (double)tp / (double)(tp + fp) : 0.0;
  double recall = tp + fn ? (double)tp / (double)(tp + fn) : 0.0;
  out->truePositives = tp;
  out->falsePositives = fp;
  out->falseNegatives = fn;
  out->precision = precision;
  out->recall = recall;
  out->f1Score = precision + recall > 0.0
                     ? 2.0 * precision * recall / (precision + recall)
                     : 0.0;
  return VT_STATUS_OK;
}