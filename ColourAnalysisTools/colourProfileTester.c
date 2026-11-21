#define WIN32_LEAN_AND_MEAN
#include "colourProfileTester.h"

BOOL colourProfileTesterInspect(int x, int y, PixelColor expectedColor,
                                PixelColourProfileReport *report) {
  if (report == NULL)
    return FALSE;
  ZeroMemory(report, sizeof(*report));

  POINT point;
  point.x = x;
  point.y = y;
  HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONULL);
  if (monitor == NULL)
    return FALSE;

  MONITORINFOEXA monitorInfo;
  ZeroMemory(&monitorInfo, sizeof(monitorInfo));
  monitorInfo.cbSize = sizeof(monitorInfo);
  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&monitorInfo))
    return FALSE;

  HDC displayDC = CreateDCA("DISPLAY", monitorInfo.szDevice, NULL, NULL);
  if (displayDC == NULL)
    return FALSE;

  DWORD profileSize = MAX_PATH;
  BOOL profileFound =
      GetICMProfileA(displayDC, &profileSize, report->iccProfilePath);

  report->iccProfileAvailable = profileFound;
  if (!profileFound) {
    report->iccProfilePath[0] = '\0';
  } else {
    report->iccProfilePath[MAX_PATH - 1] = '\0';
  }

  report->bitsPerPixel =
      GetDeviceCaps(displayDC, BITSPIXEL) * GetDeviceCaps(displayDC, PLANES);
  report->monitorBounds = monitorInfo.rcMonitor;
  CopyMemory(report->monitorName, monitorInfo.szDevice,
             sizeof(report->monitorName));
  report->monitorName[CCHDEVICENAME - 1] = '\0';
  DeleteDC(displayDC);

  if (!pixelReadColorAt(x, y, &report->sampledColor))
    return FALSE;
  report->expectedColor = expectedColor;
  report->rgbDistance = pixelColorDistance(report->sampledColor, expectedColor);
  return TRUE;
}

BOOL colourProfileTesterCompare(const PixelColourProfileReport *first,
                                const PixelColourProfileReport *second,
                                PixelColourProfileComparison *comparison) {
  if (first == NULL || second == NULL || comparison == NULL)
    return FALSE;

  comparison->sampledColourDistance =
      pixelColorDistance(first->sampledColor, second->sampledColor);
  comparison->firstDetectionError = first->rgbDistance;
  comparison->secondDetectionError = second->rgbDistance;
  comparison->detectionErrorDifference =
      first->rgbDistance - second->rgbDistance;
  comparison->sameIccProfile =
      first->iccProfileAvailable && second->iccProfileAvailable &&
      lstrcmpiA(first->iccProfilePath, second->iccProfilePath) == 0;
  comparison->sameBitDepth = first->bitsPerPixel == second->bitsPerPixel;
  return TRUE;
}