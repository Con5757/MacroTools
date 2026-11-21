#define WIN32_LEAN_AND_MEAN
#include "windowCapture.h"
BOOL windowCapture(HWND window, CaptureImage *image) {
  return captureImageCaptureWindow(window, image);
}