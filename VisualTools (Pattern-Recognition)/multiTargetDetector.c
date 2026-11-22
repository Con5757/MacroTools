#include "multiTargetDetector.h"

VTStatus vtDetectMultipleTargets(const VTImage *image, VTRect region,
                                 const VTTargetDefinition *targets,
                                 size_t count, int step, VTTargetMatch *matches,
                                 size_t capacity, size_t *matchCount) {
  if (!vtImageIsValid(image) || targets == NULL || count == 0 ||
      matches == NULL || capacity < count || matchCount == NULL || step < 1)
    return VT_STATUS_INVALID_ARGUMENT;
  size_t found = 0;
  for (size_t i = 0; i < count; ++i) {
    if (!vtImageIsValid(targets[i].templateImage) ||
        targets[i].minimumSimilarity < 0.0 ||
        targets[i].minimumSimilarity > 1.0)
      return VT_STATUS_INVALID_ARGUMENT;
    double score;
    VTPoint p;
    VTStatus s = vtImageFindTemplate(image, targets[i].templateImage, region,
                                     step, &score, &p);
    if (s == VT_STATUS_OK && score >= targets[i].minimumSimilarity) {
      matches[found++] =
          (VTTargetMatch){i,
                          p,
                          {p.x, p.y, targets[i].templateImage->width,
                           targets[i].templateImage->height},
                          score,
                          targets[i].userData};
    }
  }
  *matchCount = found;
  return found ? VT_STATUS_OK : VT_STATUS_NOT_FOUND;
}