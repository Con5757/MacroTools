#define WIN32_LEAN_AND_MEAN
#include "keyboardLayoutInspector.h"

#ifndef TU_NO_STATE_CHANGE
#define TU_NO_STATE_CHANGE 0x04
#endif

BOOL keyboardLayoutInspectorCapture(KeyboardLayoutInfo *info) {
  if (info == NULL)
    return FALSE;
  ZeroMemory(info, sizeof(*info));

  info->layout = GetKeyboardLayout(0);
  if (!GetKeyboardLayoutNameW(info->layoutName))
    return FALSE;
  info->keyboardType = (UINT)GetKeyboardType(0);
  info->keyboardSubtype = (UINT)GetKeyboardType(1);
  info->functionKeyCount = (UINT)GetKeyboardType(2);
  return TRUE;
}

static int translateKey(HKL layout, WORD virtualKey, UINT scanCode,
                        BOOL shifted, WCHAR *output, int outputCapacity) {
  BYTE state[256] = {0};
  if (shifted)
    state[VK_SHIFT] = 0x80;
  return ToUnicodeEx(virtualKey, scanCode, state, output, outputCapacity,
                     TU_NO_STATE_CHANGE, layout);
}

BOOL keyboardLayoutInspectorGetMapping(WORD virtualKey,
                                       KeyboardKeyMapping *mapping) {
  if (mapping == NULL || virtualKey == 0 || virtualKey > 255)
    return FALSE;
  ZeroMemory(mapping, sizeof(*mapping));

  HKL layout = GetKeyboardLayout(0);
  UINT scanCode = MapVirtualKeyExW(virtualKey, MAPVK_VK_TO_VSC_EX, layout);
  mapping->virtualKey = virtualKey;
  mapping->scanCode = scanCode;
  mapping->reverseMappedVirtualKey =
      MapVirtualKeyExW(scanCode, MAPVK_VSC_TO_VK_EX, layout);

  mapping->unshiftedLength =
      translateKey(layout, virtualKey, scanCode, FALSE, mapping->unshifted,
                   (int)(sizeof(mapping->unshifted) / sizeof(WCHAR)));
  mapping->shiftedLength =
      translateKey(layout, virtualKey, scanCode, TRUE, mapping->shifted,
                   (int)(sizeof(mapping->shifted) / sizeof(WCHAR)));
  return TRUE;
}
