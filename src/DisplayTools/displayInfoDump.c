#define WIN32_LEAN_AND_MEAN
#include "displayInfoDump.h"
#include <Windows.h>
#include <stdio.h>

static void printDeviceFlags(DWORD flags) {
  printf("State Flags: 0x%08lX\n", flags);

  if (flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
    printf("  Attached to Desktop\n");
  if (flags & DISPLAY_DEVICE_PRIMARY_DEVICE)
    printf("  Primary Display\n");
  if (flags & DISPLAY_DEVICE_MIRRORING_DRIVER)
    printf("  Mirroring Driver\n");
  if (flags & DISPLAY_DEVICE_VGA_COMPATIBLE)
    printf("  VGA Compatible\n");
  if (flags & DISPLAY_DEVICE_REMOVABLE)
    printf("  Removable Device\n");
  if (flags & DISPLAY_DEVICE_MODESPRUNED)
    printf("  Display Modes Pruned\n");
  if (flags & DISPLAY_DEVICE_REMOTE)
    printf("  Remote Device\n");
  if (flags & DISPLAY_DEVICE_DISCONNECT)
    printf("  Disconnected\n");
}

static void printDisplayMode(const char *deviceName) {
  DEVMODEA mode;

  ZeroMemory(&mode, sizeof(mode));
  mode.dmSize = sizeof(mode);

  if (!EnumDisplaySettingsA(deviceName, ENUM_CURRENT_SETTINGS, &mode)) {
    printf("Current display mode unavailable.\n");
    return;
  }

  printf("Resolution: %lu x %lu\n", mode.dmPelsWidth, mode.dmPelsHeight);

  printf("Refresh Rate: %lu Hz\n", mode.dmDisplayFrequency);
  printf("Bits Per Pixel: %lu\n", mode.dmBitsPerPel);
  printf("Position: X=%ld Y=%ld\n", mode.dmPosition.x, mode.dmPosition.y);

  printf("Orientation: %lu\n", mode.dmDisplayOrientation);
  printf("Display Flags: 0x%08lX\n", mode.dmDisplayFlags);
}

static void dumpMonitorDevices(const char *deviceName) {
  DISPLAY_DEVICEA monitor;
  DWORD index = 0;

  while (TRUE) {
    ZeroMemory(&monitor, sizeof(monitor));
    monitor.cb = sizeof(monitor);

    if (!EnumDisplayDevicesA(deviceName, index, &monitor, 0)) {
      break;
    }

    printf("\n  Monitor Device %lu\n", index + 1);
    printf("  Device Name: %s\n", monitor.DeviceName);
    printf("  Description: %s\n", monitor.DeviceString);
    printf("  Device ID: %s\n", monitor.DeviceID);
    printf("  Device Key: %s\n", monitor.DeviceKey);

    printf("  ");
    printDeviceFlags(monitor.StateFlags);

    index++;
  }
}

void displayInfoDump(void) {
  DISPLAY_DEVICEA device;
  DWORD index = 0;
  DWORD deviceCount = 0;

  printf("           DISPLAY INFO DUMP\n");

  while (TRUE) {
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);

    if (!EnumDisplayDevicesA(NULL, index, &device, 0)) {
      break;
    }

    printf("Display Device %lu\n", index + 1);

    printf("Device Name: %s\n", device.DeviceName);
    printf("Description: %s\n", device.DeviceString);
    printf("Device ID: %s\n", device.DeviceID);
    printf("Device Key: %s\n", device.DeviceKey);

    printDeviceFlags(device.StateFlags);

    if (device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {
      printf("\nCurrent Display Mode\n");
      printDisplayMode(device.DeviceName);
    }

    dumpMonitorDevices(device.DeviceName);

    deviceCount++;
    index++;
  }

  printf("Total Display Devices: %lu\n", deviceCount);
}