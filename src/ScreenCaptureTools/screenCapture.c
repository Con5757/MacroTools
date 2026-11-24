#define WIN32_LEAN_AND_MEAN
#include "screenCapture.h"
#include "screenCapture/fullDesktopCapture.h"
#include "screenCapture/monitorCapture.h"
#include "screenCapture/regionCapture.h"
#include "screenCapture/windowCapture.h"

BOOL screenCaptureFullDesktop(CaptureImage *image) {
  return fullDesktopCapture(image);
}
BOOL screenCaptureMonitor(HMONITOR monitor, CaptureImage *image) {
  return monitorCapture(monitor, image);
}
BOOL screenCaptureRegion(const RECT *desktopRegion, CaptureImage *image) {
  return regionCapture(desktopRegion, image);
}
BOOL screenCaptureWindow(HWND window, CaptureImage *image) {
  return windowCapture(window, image);
}