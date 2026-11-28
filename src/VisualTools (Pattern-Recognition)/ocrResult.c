#include "ocrResult.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static int vtOcrElementTypeIsValid(VTOcrElementType type) {
  return type >= VT_OCR_ELEMENT_BLOCK && type <= VT_OCR_ELEMENT_SYMBOL;
}
static int vtOcrExpectedParentType(VTOcrElementType type) {
  switch (type) {
  case VT_OCR_ELEMENT_PARAGRAPH:
    return VT_OCR_ELEMENT_BLOCK;

  case VT_OCR_ELEMENT_LINE:
    return VT_OCR_ELEMENT_PARAGRAPH;

  case VT_OCR_ELEMENT_WORD:
    return VT_OCR_ELEMENT_LINE;

  case VT_OCR_ELEMENT_SYMBOL:
    return VT_OCR_ELEMENT_WORD;

  default:
    return -1;
  }
}
void vtOcrResultInitialize(VTOcrResult *result) {
  if (result == NULL) {
    return;
  }

  result->elements = NULL;
  result->count = 0;
  result->capacity = 0;
}
VTStatus vtOcrResultAdd(VTOcrResult *result, VTOcrElementType type,
                        VTRect bounds, float confidence, const char *utf8Text,
                        size_t parentIndex, size_t *elementIndex) {
  if (result == NULL || utf8Text == NULL || !vtOcrElementTypeIsValid(type) ||
      !vtRectIsValid(bounds) || result->count > result->capacity ||
      (result->count > 0 && result->elements == NULL)) {
    return VT_STATUS_INVALID_ARGUMENT;
  }
  if (!isfinite(confidence) || (confidence != VT_OCR_CONFIDENCE_UNAVAILABLE &&
                                (confidence < 0.0f || confidence > 100.0f))) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  int expectedParentType = vtOcrExpectedParentType(type);

  if (type == VT_OCR_ELEMENT_BLOCK) {
    if (parentIndex != VT_OCR_NO_PARENT) {
      return VT_STATUS_INVALID_ARGUMENT;
    }
  } else {
    if (parentIndex == VT_OCR_NO_PARENT || parentIndex >= result->count ||
        result->elements[parentIndex].type != expectedParentType) {
      return VT_STATUS_INVALID_ARGUMENT;
    }
  }

  size_t textLength = strlen(utf8Text);
  if (textLength == SIZE_MAX) {
    return VT_STATUS_OUT_OF_MEMORY;
  }

  char *textCopy = malloc(textLength + 1);
  if (textCopy == NULL) {
    return VT_STATUS_OUT_OF_MEMORY;
  }

  memcpy(textCopy, utf8Text, textLength + 1);

  if (result->count == result->capacity) {
    size_t newCapacity = result->capacity == 0 ? 8 : result->capacity * 2;
    if (newCapacity < result->capacity ||
        newCapacity > SIZE_MAX / sizeof(VTOcrElement)) {
      free(textCopy);
      return VT_STATUS_OUT_OF_MEMORY;
    }

    VTOcrElement *newElements =
        realloc(result->elements, newCapacity * sizeof(VTOcrElement));

    if (newElements == NULL) {
      free(textCopy);
      return VT_STATUS_OUT_OF_MEMORY;
    }

    result->elements = newElements;
    result->capacity = newCapacity;
  }

  size_t newIndex = result->count;
  result->elements[newIndex] = (VTOcrElement){.type = type,
                                              .bounds = bounds,
                                              .confidence = confidence,
                                              .utf8Text = textCopy,
                                              .parentIndex = parentIndex};

  result->count++;

  if (elementIndex != NULL) {
    *elementIndex = newIndex;
  }

  return VT_STATUS_OK;
}
void vtOcrResultDestroy(VTOcrResult *result) {
  if (result == NULL) {
    return;
  }

  for (size_t i = 0; i < result->count; i++) {
    free(result->elements[i].utf8Text);
  }

  free(result->elements);

  result->elements = NULL;
  result->count = 0;
  result->capacity = 0;
}