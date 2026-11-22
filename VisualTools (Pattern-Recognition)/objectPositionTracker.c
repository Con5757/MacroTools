#include "objectPositionTracker.h"

VTStatus vtObjectTrackerInitialize(VTObjectTracker *t, const VTImage *templ,
                                   int radius, int step, double threshold) {
  if (t == NULL || !vtImageIsValid(templ) || radius < 0 || step < 1 ||
      threshold < 0.0 || threshold > 1.0)
    return VT_STATUS_INVALID_ARGUMENT;
  t->templateImage = templ;
  t->lastPosition = (VTPoint){0, 0};
  t->searchRadius = radius;
  t->step = step;
  t->minimumSimilarity = threshold;
  t->initialized = FALSE;
  return VT_STATUS_OK;
}
VTStatus vtObjectTrackerUpdate(VTObjectTracker *t, const VTImage *frame,
                               VTObjectTrackResult *out) {
  if (t == NULL || !vtImageIsValid(frame) || out == NULL ||
      !vtImageIsValid(t->templateImage))
    return VT_STATUS_INVALID_ARGUMENT;
  VTRect region = {0, 0, frame->width, frame->height};
  if (t->initialized && t->searchRadius > 0) {
    int left = t->lastPosition.x - t->searchRadius,
        top = t->lastPosition.y - t->searchRadius;
    int right = t->lastPosition.x + t->templateImage->width + t->searchRadius;
    int bottom = t->lastPosition.y + t->templateImage->height + t->searchRadius;
    if (left < 0)
      left = 0;
    if (top < 0)
      top = 0;
    if (right > frame->width)
      right = frame->width;
    if (bottom > frame->height)
      bottom = frame->height;
    region = (VTRect){left, top, right - left, bottom - top};
  }
  double score;
  VTPoint point;
  VTStatus s = vtImageFindTemplate(frame, t->templateImage, region, t->step,
                                   &score, &point);
  if (s != VT_STATUS_OK) {
    out->found = FALSE;
    return s;
  }
  out->position = point;
  out->bounds = (VTRect){point.x, point.y, t->templateImage->width,
                         t->templateImage->height};
  out->similarity = score;
  out->found = score >= t->minimumSimilarity;
  if (out->found) {
    t->lastPosition = point;
    t->initialized = TRUE;
  }
  return out->found ? VT_STATUS_OK : VT_STATUS_NOT_FOUND;
}