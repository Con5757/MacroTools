#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  VT_SHAPE_UNKNOWN,
  VT_SHAPE_RECTANGLE,
  VT_SHAPE_ELLIPSE,
  VT_SHAPE_LINE,
  VT_SHAPE_BLOB
} VTShapeType;
typedef struct {
  VTRect bounds;
  VTShapeType type;
  size_t pixels;
  double fillRatio;
} VTDetectedShape;

VTStatus vtDetectShapes(const VTImage *image, uint8_t brightnessThreshold,
                        size_t minimumPixels, VTDetectedShape *shapes,
                        size_t capacity, size_t *shapeCount);

#ifdef __cplusplus
}
#endif