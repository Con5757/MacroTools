#define WIN32_LEAN_AND_MEAN
#include "mouseSensitivityAnalyzer.h"

#include <math.h>
#include <stdlib.h>

void mouseSensitivityAnalyzerInitialize(MouseSensitivityAnalyzer *analyzer) {
  if (analyzer != NULL) {
    ZeroMemory(analyzer, sizeof(*analyzer));
  }
}

BOOL mouseSensitivityAnalyzerRegister(MouseSensitivityAnalyzer *analyzer,
                                      HWND targetWindow,
                                      BOOL captureInBackground) {
  if (analyzer == NULL || targetWindow == NULL) {
    return FALSE;
  }

  RAWINPUTDEVICE device;
  device.usUsagePage = 0x01;
  device.usUsage = 0x02;
  device.dwFlags = captureInBackground ? RIDEV_INPUTSINK : 0;
  device.hwndTarget = targetWindow;

  if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
    return FALSE;
  }

  analyzer->targetWindow = targetWindow;
  analyzer->isRegistered = TRUE;
  analyzer->hasPreviousCursorPosition = FALSE;
  return TRUE;
}

BOOL mouseSensitivityAnalyzerHandleMessage(MouseSensitivityAnalyzer *analyzer,
                                           UINT message, LPARAM lParam) {
  if (analyzer == NULL || message != WM_INPUT || !analyzer->isRegistered) {
    return FALSE;
  }

  UINT dataSize = 0;
  if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dataSize,
                      sizeof(RAWINPUTHEADER)) == (UINT)-1 ||
      dataSize == 0) {
    return FALSE;
  }

  BYTE *buffer = (BYTE *)malloc(dataSize);
  if (buffer == NULL) {
    return FALSE;
  }

  UINT received = dataSize;
  UINT result = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &received,
                                sizeof(RAWINPUTHEADER));
  if (result == (UINT)-1 || result != dataSize) {
    free(buffer);
    return FALSE;
  }

  RAWINPUT *raw = (RAWINPUT *)buffer;
  if (raw->header.dwType != RIM_TYPEMOUSE) {
    free(buffer);
    return FALSE;
  }

  if ((raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) != 0) {
    analyzer->ignoredAbsoluteEventCount++;
    free(buffer);
    return TRUE;
  }

  LONG rawX = raw->data.mouse.lLastX;
  LONG rawY = raw->data.mouse.lLastY;
  analyzer->totalRawDeltaX += rawX;
  analyzer->totalRawDeltaY += rawY;
  analyzer->totalRawDistance += sqrt((double)rawX * rawX + (double)rawY * rawY);
  analyzer->rawEventCount++;

  POINT cursor;
  if (GetCursorPos(&cursor)) {
    if (analyzer->hasPreviousCursorPosition) {
      LONG dx = cursor.x - analyzer->previousCursorPosition.x;
      LONG dy = cursor.y - analyzer->previousCursorPosition.y;
      analyzer->totalCursorDistance += sqrt((double)dx * dx + (double)dy * dy);
    }
    analyzer->previousCursorPosition = cursor;
    analyzer->hasPreviousCursorPosition = TRUE;
  }

  analyzer->pixelsPerRawCount =
      analyzer->totalRawDistance > 0.0
          ? analyzer->totalCursorDistance / analyzer->totalRawDistance
          : 0.0;

  free(buffer);
  return TRUE;
}

void mouseSensitivityAnalyzerReset(MouseSensitivityAnalyzer *analyzer) {
  if (analyzer == NULL) {
    return;
  }

  analyzer->totalRawDeltaX = 0;
  analyzer->totalRawDeltaY = 0;
  analyzer->totalRawDistance = 0.0;
  analyzer->totalCursorDistance = 0.0;
  analyzer->pixelsPerRawCount = 0.0;
  analyzer->rawEventCount = 0;
  analyzer->ignoredAbsoluteEventCount = 0;
  analyzer->hasPreviousCursorPosition = FALSE;
}

void mouseSensitivityAnalyzerUnregister(MouseSensitivityAnalyzer *analyzer) {
  if (analyzer == NULL || !analyzer->isRegistered) {
    return;
  }

  RAWINPUTDEVICE device;
  device.usUsagePage = 0x01;
  device.usUsage = 0x02;
  device.dwFlags = RIDEV_REMOVE;
  device.hwndTarget = NULL;
  RegisterRawInputDevices(&device, 1, sizeof(device));

  analyzer->isRegistered = FALSE;
  analyzer->targetWindow = NULL;
}
