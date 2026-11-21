# Screen Capture & Image Acquisition Tools

Windows-only C modules for desktop capture, image acquisition, comparison, and capture diagnostics. Every tool is an independently callable module; there are no `main()` functions and no console loops. The APIs are intended to be driven by a host application or a Raylib update loop.

## Directory layout

```text
ScreenCaptureTools/
├── common/
│   ├── captureImage.c/.h
│   └── captureWic.c/.h
├── screenCapture.c/.h
├── screenCapture/
│   ├── fullDesktopCapture.c/.h
│   ├── monitorCapture.c/.h
│   ├── regionCapture.c/.h
│   └── windowCapture.c/.h
├── continuousScreenCapture.c/.h
├── screenshotRecorder.c/.h
├── captureFormatConverter.c/.h
├── imageComparisonTool.c/.h
├── screenChangeDetector.c/.h
├── regionChangeDetector.c/.h
├── screenCaptureProfiler.c/.h
└── captureConfigurationInspector.c/.h
```

## Tools and responsibilities

- **Screen Capture**: Parent interface for full-desktop, individual-monitor, region, and application-window capture. Each sub-bullet has its own implementation and header in `screenCapture/`.
- **Continuous Screen Capture**: Interval-based, non-blocking capture for desktop, monitor, region, or window sources.
- **Screenshot Recorder**: Saves sequential full-desktop captures with local timestamps and sequence numbers.
- **Capture Format Converter**: Loads supported WIC image formats and writes PNG, JPEG, or BMP.
- **Image Comparison Tool**: Compares same-sized BGRA frames, computes changed-pixel percentage and mean absolute error, and can produce a difference image.
- **Screen Change Detector**: Compares each new frame against the previous frame and reports whether the configured percentage changed.
- **Region Change Detector**: Performs the same operation within an image-relative rectangle.
- **Screen Capture Profiler**: Measures capture callback duration using QueryPerformanceCounter and reports minimum, maximum, and average timings.
- **Capture Configuration Inspector**: Enumerates monitor bounds, work areas, primary status, virtual-desktop dimensions, and image metadata.

## Image ownership

`CaptureImage` owns its `pixels` allocation. Always initialize an image as `CaptureImage image = {0};`, and release it with `captureImageFree(&image)`. Functions that write an image expect an empty/zero-initialized destination or a previously valid `CaptureImage` managed by this library. Do not shallow-copy `CaptureImage`; use `captureImageClone()` when an independent copy is needed.

Pixels use top-down 32-bit BGRA layout. Coordinates for desktop regions are virtual-desktop coordinates, so monitors positioned left or above the primary display may have negative coordinates. Region-change rectangles are relative to the captured image's top-left corner.

## Basic use

```c
#include "screenCapture.h"
#include "common/captureWic.h"

CaptureImage frame = {0};
if (screenCaptureFullDesktop(&frame)) {
    captureImageSave("desktop.png", CAPTURE_FORMAT_PNG, &frame);
}
captureImageFree(&frame);
```

For a Raylib loop, call `continuousScreenCaptureUpdate()` once per frame. It returns immediately when the interval has not elapsed and does not call `Sleep()`.

## Build requirements

- Windows 10 or later recommended
- C11 compiler (MinGW-w64 GCC or MSVC)
- Windows GDI/User32 APIs
- Windows Imaging Component (WIC) for PNG/JPEG/BMP load and save

Link these Windows libraries:

```text
user32 gdi32 ole32 windowscodecs uuid
```

Example MinGW-w64 compilation of all source files into one object build (run from this folder):

```powershell
$files = Get-ChildItem -Recurse -Filter *.c | ForEach-Object { $_.FullName }
gcc -Wall -Wextra -std=c11 -I. -c $files
```

When linking the resulting objects into an executable, add:

```powershell
-lgdi32 -luser32 -lole32 -lwindowscodecs -luuid
```

## Integration notes and limitations

- The host process should be Per-Monitor DPI Aware (preferably Per-Monitor V2) so GDI coordinates align with physical pixels. Set DPI awareness once in the application startup, not independently in these modules.
- Full desktop capture uses the virtual-desktop bounding rectangle. Spaces between non-contiguous monitors are part of that rectangle and may appear as empty desktop pixels.
- Window capture first uses `PrintWindow`; some GPU-accelerated, protected, minimized, or unusual windows may not render correctly. The fallback captures currently visible desktop pixels in the window rectangle and therefore cannot capture obscured content.
- Secure desktops, protected video, elevated processes with incompatible access, and some hardware overlays may not be capturable through GDI.
- WIC supports PNG, JPEG, and BMP output here. JPEG is lossy; PNG and BMP are lossless. Image paths passed to WIC are UTF-8.
- `screenshotRecorderStart()` creates the target directory if it does not exist, but only creates that directory level; its parent directory must already exist.
- `continuousScreenCaptureUpdate()` and recorder update functions are non-blocking with respect to their configured interval, but an actual capture and image encoding operation still takes time. For expensive encoding, call the recorder on a worker thread managed by the host application.
- These sources are prepared for Windows integration but have not been runtime-tested against your machine or Raylib host. Validate capture behavior on Windows, especially mixed-DPI and GPU-rendered windows.