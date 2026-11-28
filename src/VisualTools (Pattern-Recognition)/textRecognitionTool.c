#include "textRecognitionTool.h"

static int vtTextRegionIsValid(const VTImage *image, VTRect region) {
  return vtImageIsValid(image) && vtRectIsValid(region) && region.x >= 0 &&
         region.y >= 0 && region.x <= image->width - region.width &&
         region.y <= image->height - region.height;
}
VTStatus vtTextRecognize(const VTImage *image, VTRect region,
                         VTOcrEngine engine, void *userData, char *utf8Output,
                         size_t outputCapacity) {
  if (!vtTextRegionIsValid(image, region) || utf8Output == NULL ||
      outputCapacity == 0) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  utf8Output[0] = '\0';

  if (engine == NULL) {
    return VT_STATUS_UNSUPPORTED;
  }

  VTStatus status = engine(image, region, utf8Output, outputCapacity, userData);
  utf8Output[outputCapacity - 1] = '\0';

  return status;
}
VTStatus vtTextRecognizeStructured(const VTImage *image, VTRect region,
                                   VTOcrStructuredEngine engine, void *userData,
                                   VTOcrResult *result) {
  if (result == NULL) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  vtOcrResultInitialize(result);

  if (!vtTextRegionIsValid(image, region)) {
    return VT_STATUS_INVALID_ARGUMENT;
  }
  if (engine == NULL) {
    return VT_STATUS_UNSUPPORTED;
  }

  VTOcrResult temporary;
  vtOcrResultInitialize(&temporary);

  VTStatus status = engine(image, region, &temporary, userData);

  if (status != VT_STATUS_OK) {
    vtOcrResultDestroy(&temporary);
    return status;
  }
  if (temporary.count > temporary.capacity ||
      (temporary.count > 0 && temporary.elements == NULL)) {
    vtOcrResultDestroy(&temporary);
    return VT_STATUS_INVALID_ARGUMENT;
  }

  *result = temporary;

  return VT_STATUS_OK;
}