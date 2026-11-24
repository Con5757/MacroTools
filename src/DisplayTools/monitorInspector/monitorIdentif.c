#define WIN32_LEAN_AND_MEAN
#include "monitorIdentif.h"
#include <Windows.h>
#include <stdio.h>

static BOOL CALLBACK monitorIdentifCallback(HMONITOR monitor, HDC dc,
                                            LPRECT rect, LPARAM data) {
  MONITORINFOEXA info;
  DISPLAY_DEVICEA device;
  DWORD index = 0;

  ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);

  if (!GetMonitorInfoA(monitor, (LPMONITORINFO)&info)) {
    return TRUE;
  }

  printf("\n%s\n", info.szDevice);

  while (TRUE) {
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);

    if (!EnumDisplayDevicesA(info.szDevice, index, &device, 0)) {
      break;
    }

    printf("Device Name: %s\n", device.DeviceName);
    printf("Description: %s\n", device.DeviceString);
    printf("Device ID: %s\n", device.DeviceID);
    printf("Device Key: %s\n", device.DeviceKey);
    printf("State Flags: 0x%08lX\n", device.StateFlags);

    index++;
  }

  return TRUE;
}

void monitorIdentif(void) {
  printf("\nMonitor Identification\n");

  if (!EnumDisplayMonitors(NULL, NULL, monitorIdentifCallback, 0)) {
    printf("Monitor identification failed.\n");
  }
}