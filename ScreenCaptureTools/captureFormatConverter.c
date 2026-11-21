#define WIN32_LEAN_AND_MEAN
#include "captureFormatConverter.h"

BOOL captureFormatConvertFile(const char *inputPathUtf8,
                              const char *outputPathUtf8,
                              CaptureImageFormat outputFormat) {
  if (inputPathUtf8 == NULL || outputPathUtf8 == NULL ||
      captureImageFormatExtension(outputFormat) == NULL)
    return FALSE;
  CaptureImage image = {0};
  BOOL success = captureImageLoad(inputPathUtf8, &image) &&
                 captureImageSave(outputPathUtf8, outputFormat, &image);
  captureImageFree(&image);
  return success;
}