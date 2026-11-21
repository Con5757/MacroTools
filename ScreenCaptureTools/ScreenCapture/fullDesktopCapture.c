#define WIN32_LEAN_AND_MEAN
#include "fullDesktopCapture.h"

BOOL fullDesktopCapture(CaptureImage *image) {
  if (image == NULL)
    return FALSE;
  RECT virtualDesktop;
  virtualDesktop.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  virtualDesktop.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  virtualDesktop.right =
      virtualDesktop.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
  virtualDesktop.bottom =
      virtualDesktop.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
  return captureImageCaptureRect(&virtualDesktop, image);
}