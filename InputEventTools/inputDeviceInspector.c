#include "inputDeviceInspector.h"
#include <stdlib.h>
#include <string.h>

BOOL inputDeviceInspectorEnumerate(InputDeviceInfo *devices, size_t capacity,
                                   size_t *count) {
  if (count == NULL || (capacity != 0 && devices == NULL))
    return FALSE;
  *count = 0;
  UINT rawCount = 0;
  if (GetRawInputDeviceList(NULL, &rawCount, sizeof(RAWINPUTDEVICELIST)) ==
      (UINT)-1)
    return FALSE;
  if (rawCount == 0)
    return TRUE;
  RAWINPUTDEVICELIST *list =
      (RAWINPUTDEVICELIST *)calloc(rawCount, sizeof(*list));
  if (list == NULL)
    return FALSE;
  UINT filled = rawCount;
  if (GetRawInputDeviceList(list, &filled, sizeof(RAWINPUTDEVICELIST)) ==
      (UINT)-1) {
    free(list);
    return FALSE;
  }
  *count = filled;
  BOOL complete = capacity >= filled;
  size_t limit = capacity < filled ? capacity : filled;
  for (size_t i = 0; i < limit; ++i) {
    devices[i].handle = list[i].hDevice;
    devices[i].type = list[i].dwType == RIM_TYPEMOUSE ? INPUT_DEVICE_MOUSE
                      : list[i].dwType == RIM_TYPEKEYBOARD
                          ? INPUT_DEVICE_KEYBOARD
                          : INPUT_DEVICE_OTHER;
    UINT nameChars = INPUT_DEVICE_NAME_CAPACITY;
    devices[i].name[0] = '\0';
    if (GetRawInputDeviceInfoA(list[i].hDevice, RIDI_DEVICENAME,
                               devices[i].name, &nameChars) == (UINT)-1) {
      strncpy(devices[i].name, "<unavailable>", sizeof(devices[i].name) - 1);
      devices[i].name[sizeof(devices[i].name) - 1] = '\0';
    } else {
      devices[i].name[sizeof(devices[i].name) - 1] = '\0';
    }
  }
  free(list);
  return complete;
}