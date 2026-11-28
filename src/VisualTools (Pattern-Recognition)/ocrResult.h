#pragma once

#include "visualToolsCommon.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VT_OCR_NO_PARENT SIZE_MAX
#define VT_OCR_CONFIDENCE_UNAVAILABLE (-1.0f)

typedef enum {
  VT_OCR_ELEMENT_BLOCK = 0,
  VT_OCR_ELEMENT_PARAGRAPH,
  VT_OCR_ELEMENT_LINE,
  VT_OCR_ELEMENT_WORD,
  VT_OCR_ELEMENT_SYMBOL
} VTOcrElementType;
typedef struct {
  VTOcrElementType type;
  VTRect bounds;
  float confidence;
  char *utf8Text;
  size_t parentIndex;
} VTOcrElement;
typedef struct {
  VTOcrElement *elements;
  size_t count;
  size_t capacity;
} VTOcrResult;

void vtOcrResultInitialize(VTOcrResult *result);
VTStatus vtOcrResultAdd(VTOcrResult *result, VTOcrElementType type,
                        VTRect bounds, float confidence, const char *utf8Text,
                        size_t parentIndex, size_t *elementIndex);
void vtOcrResultDestroy(VTOcrResult *result);

#ifdef __cplusplus
}
#endif