#pragma once
#include "captureImage.h"

typedef enum {
  CAPTURE_FORMAT_PNG = 0,
  CAPTURE_FORMAT_JPEG = 1,
  CAPTURE_FORMAT_BMP = 2
} CaptureImageFormat;

BOOL captureImageSave(const char *utf8Path, CaptureImageFormat format,
                      const CaptureImage *image);
BOOL captureImageLoad(const char *utf8Path, CaptureImage *image);
const char *captureImageFormatExtension(CaptureImageFormat format);