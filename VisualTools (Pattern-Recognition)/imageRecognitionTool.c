#include "imageRecognitionTool.h"
#include "templateMatcher.h"

VTStatus vtImageRecognize(const VTImage *image, VTRect region,
                          const VTRecognitionClass *classes, size_t count,
                          int step, VTRecognitionResult *result) {
  if (!vtImageIsValid(image) || classes == NULL || count == 0 ||
      result == NULL || step < 1)
    return VT_STATUS_INVALID_ARGUMENT;
  double best = -1.0;
  size_t bestIndex = 0;
  VTTemplateMatch bestMatch = {0};
  for (size_t i = 0; i < count; ++i) {
    if (classes[i].name == NULL || !vtImageIsValid(classes[i].templateImage) ||
        classes[i].minimumSimilarity < 0.0 ||
        classes[i].minimumSimilarity > 1.0)
      return VT_STATUS_INVALID_ARGUMENT;
    VTTemplateMatch candidate;
    VTStatus status = vtTemplateMatch(image, classes[i].templateImage, region,
                                      step, &candidate);
    if (status == VT_STATUS_OK &&
        candidate.similarity >= classes[i].minimumSimilarity &&
        candidate.similarity > best) {
      best = candidate.similarity;
      bestIndex = i;
      bestMatch = candidate;
    }
  }
  if (best < 0.0)
    return VT_STATUS_NOT_FOUND;
  result->classIndex = bestIndex;
  result->name = classes[bestIndex].name;
  result->match = bestMatch;
  return VT_STATUS_OK;
}