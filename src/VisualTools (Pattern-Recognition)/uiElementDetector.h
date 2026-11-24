#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  VT_UI_BUTTON,
  VT_UI_ICON,
  VT_UI_PANEL,
  VT_UI_UNKNOWN
} VTUIElementType;
typedef struct {
  VTRect bounds;
  VTUIElementType type;
  double confidence;
} VTUIElement;

VTStatus vtDetectUIElements(const VTImage *image, uint8_t brightnessThreshold,
                            VTUIElement *elements, size_t capacity,
                            size_t *elementCount);

#ifdef __cplusplus
}
#endif