#define WIN32_LEAN_AND_MEAN
#include "regionCapture.h"
BOOL regionCapture(const RECT *desktopRegion, CaptureImage *image) {
  return captureImageCaptureRect(desktopRegion, image);
}