#include "textRecognitionTool.h"

VTStatus vtTextRecognize(const VTImage *image, VTRect region,
                         VTOcrEngine engine, void *userData, char *output,
                         size_t capacity) {
  if (!vtImageIsValid(image) || !vtRectIsValid(region) || region.x < 0 ||
      region.y < 0 || region.x > image->width - region.width ||
      region.y > image->height - region.height || output == NULL ||
      capacity == 0)
    return VT_STATUS_INVALID_ARGUMENT;
  output[0] = '\0';
  if (engine == NULL)
    return VT_STATUS_UNSUPPORTED;
  VTStatus status = engine(image, region, output, capacity, userData);
  output[capacity - 1] = '\0';
  return status;
}