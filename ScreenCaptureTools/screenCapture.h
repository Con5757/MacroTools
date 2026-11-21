#pragma once
#include "common/captureImage.h"
#include <Windows.h>

BOOL screenCaptureFullDesktop(CaptureImage *image);
BOOL screenCaptureMonitor(HMONITOR monitor, CaptureImage *image);
BOOL screenCaptureRegion(const RECT *desktopRegion, CaptureImage *image);
BOOL screenCaptureWindow(HWND window, CaptureImage *image);