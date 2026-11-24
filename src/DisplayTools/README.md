# Display Tools

A collection of lightweight Windows display utilities written in C, using the Windows API.

The project focuses on monitor inspection, display configuration, coordinate conversion, and multi-monitor automation support.

## Tools

### Monitor Inspector
- **Monitor Enumeration**: List all connected monitors.
- **Monitor Resolution**: Retrieve monitor resolutions and refresh rates.
- **Monitor Position**: Retrieve virtual desktop coordinates and dimensions.
- **Monitor Identification**: Retrieve Windows display identifiers and device information.

### Monitor Switch Detector
Detect when the cursor moves between connected monitors.

### Monitor Config Tracker
Monitor display configuration changes, including resolution changes and device connections or disconnections.

### Display Scaling Inspector
Retrieve monitor DPI and Windows display scaling percentages.

### Resolution Detector
Continuously monitor connected display resolutions and refresh rates.

### Refresh Rate Detector
Detect refresh-rate changes and monitor connection or disconnection events.

### Coordinate Converter
- Convert desktop coordinates into monitor-relative coordinates.
- Convert monitor-relative coordinates into desktop coordinates.
- Transfer coordinates between monitors with different resolutions.

### Display Info Dump
Generate a detailed console report containing display device information, identifiers, resolutions, refresh rates, positions, and device flags.

## Requirements

- Windows
- C compiler (GCC or Clang)
- Windows API

## Compilation

Example using MinGW GCC:

```powershell
gcc -Wall -Wextra -std=c11 -o monitorInspector.exe monitorInspector.c monitorInspector/monitorEnum.c monitorInspector/monitorRes.c monitorInspector/monitorPos.c monitorInspector/monitorIdentif.c -luser32
```

Display Scaling Inspector additionally requires:

```powershell
-lShcore
```

## Project Status

Individual utilities are being developed and tested independently. The project is intended to serve as a collection of reusable display-related tools for larger automation projects.

## Language

C