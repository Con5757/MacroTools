#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

typedef struct {
  HKL layout;
  WCHAR layoutName[KL_NAMELENGTH];
  UINT keyboardType;
  UINT keyboardSubtype;
  UINT functionKeyCount;
} KeyboardLayoutInfo;

typedef struct {
  WORD virtualKey;
  UINT scanCode;
  UINT reverseMappedVirtualKey;
  WCHAR unshifted[8];
  WCHAR shifted[8];
  int unshiftedLength;
  int shiftedLength;
} KeyboardKeyMapping;

BOOL keyboardLayoutInspectorCapture(KeyboardLayoutInfo *info);
BOOL keyboardLayoutInspectorGetMapping(WORD virtualKey,
                                       KeyboardKeyMapping *mapping);
