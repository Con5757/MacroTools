#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

typedef struct {
  LONGLONG totalRawDeltaX;
  LONGLONG totalRawDeltaY;
  double totalRawDistance;
  double totalCursorDistance;
  double pixelsPerRawCount;
  ULONGLONG rawEventCount;
  ULONGLONG ignoredAbsoluteEventCount;
  POINT previousCursorPosition;
  BOOL hasPreviousCursorPosition;
  BOOL isRegistered;
  HWND targetWindow;
} MouseSensitivityAnalyzer;

void mouseSensitivityAnalyzerInitialize(MouseSensitivityAnalyzer *analyzer);
BOOL mouseSensitivityAnalyzerRegister(MouseSensitivityAnalyzer *analyzer,
                                      HWND targetWindow,
                                      BOOL captureInBackground);
BOOL mouseSensitivityAnalyzerHandleMessage(MouseSensitivityAnalyzer *analyzer,
                                           UINT message, LPARAM lParam);
void mouseSensitivityAnalyzerReset(MouseSensitivityAnalyzer *analyzer);
void mouseSensitivityAnalyzerUnregister(MouseSensitivityAnalyzer *analyzer);
