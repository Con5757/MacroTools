#include "templateMatcher.h"

VTStatus vtTemplateMatch(const VTImage *image, const VTImage *templateImage,
                         VTRect region, int step, VTTemplateMatch *match) {
  if (match == NULL)
    return VT_STATUS_INVALID_ARGUMENT;
  double score = 0.0;
  VTPoint point = {0, 0};
  VTStatus status =
      vtImageFindTemplate(image, templateImage, region, step, &score, &point);
  if (status != VT_STATUS_OK)
    return status;
  match->position = point;
  match->similarity = score;
  match->bounds =
      (VTRect){point.x, point.y, templateImage->width, templateImage->height};
  return VT_STATUS_OK;
}