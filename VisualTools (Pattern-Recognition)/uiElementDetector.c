#include "uiElementDetector.h"
#include "shapeDetector.h"
#include <stdlib.h>

VTStatus vtDetectUIElements(const VTImage *image, uint8_t threshold,
                            VTUIElement *elements, size_t capacity,
                            size_t *count) {
  if (!vtImageIsValid(image) || elements == NULL || capacity == 0 ||
      count == NULL)
    return VT_STATUS_INVALID_ARGUMENT;
  VTDetectedShape *shapes =
      (VTDetectedShape *)malloc(capacity * sizeof(*shapes));
  if (!shapes)
    return VT_STATUS_OUT_OF_MEMORY;
  size_t shapeCount = 0;
  VTStatus s =
      vtDetectShapes(image, threshold, 12, shapes, capacity, &shapeCount);
  if (s != VT_STATUS_OK) {
    free(shapes);
    return s;
  }
  size_t out = 0;
  for (size_t i = 0; i < shapeCount && out < capacity; ++i) {
    VTRect r = shapes[i].bounds;
    VTUIElementType type = VT_UI_UNKNOWN;
    if (r.width >= 36 && r.height >= 18 && r.width <= image->width / 2 &&
        r.height <= image->height / 3)
      type = VT_UI_BUTTON;
    else if (r.width <= 96 && r.height <= 96)
      type = VT_UI_ICON;
    else if (r.width > image->width / 4 && r.height > image->height / 6)
      type = VT_UI_PANEL;
    if (type != VT_UI_UNKNOWN) {
      double sizePenalty =
          (double)shapes[i].pixels / ((double)r.width * r.height);
      elements[out++] = (VTUIElement){r, type, sizePenalty};
    }
  }
  free(shapes);
  *count = out;
  return VT_STATUS_OK;
}