# Pixel Tools

A collection of Windows pixel and colour inspection utilities written in C.
The modules are designed to be called by a host application (including a future
Raylib GUI); none of the modules defines `main()` or owns an application loop.

## Modules

| Module | Purpose |
|---|---|
| `pixelInspector` | Read one screen coordinate and return RGB, HEX, HSV, HSL and CMYK values. |
| `pixelTracker` | Sample one coordinate at a configurable interval. |
| `multiPixelTracker` | Sample multiple coordinates on the same interval. |
| `colourFinder` | Find pixels matching a target RGB colour within an RGB Euclidean tolerance. |
| `colourRangeFinder` | Search using inclusive RGB ranges or HSV ranges (including hue ranges that wrap through 0 degrees). |
| `colourSampler` | Return average, minimum, maximum and centre colours for a region. |
| `colourChangeDetector` | Detect sampled pixel changes that meet a configurable RGB distance threshold. |
| `colourDifferenceAnalyzer` | Compare two colours using signed RGB differences, RGB distances and CIE76 Delta E. |
| `colourThresholdTester` | Test per-channel or Euclidean RGB tolerances. |
| `colourClusterDetector` | Find 4-connected groups of pixels near a target colour. |
| `colourFrequencyAnalyzer` | Rank the most frequent colours; optional per-channel quantization groups similar shades. |
| `pixelGridExporter` | Write a captured region as per-pixel CSV or a hexadecimal colour grid. |
| `pixelHistoryRecorder` | Record a pixel's colour history with elapsed-millisecond timestamps. |
| `colourProfileTester` | Inspect monitor identity, active GDI ICC profile and bit depth, and compare a sampled pixel with an expected colour. |

## Coordinate and colour conventions

- All screen coordinates use the Windows virtual-desktop coordinate system.
- The primary monitor commonly begins at `(0, 0)`; monitors positioned to its
  left or above can have negative coordinates.
- `PixelColor` stores 8-bit red, green and blue channels in that order.
- HSV hue is measured in degrees (`0..360`); HSV saturation/value and HSL
  saturation/lightness are normalized (`0..1`). CMYK channels are normalized
  (`0..1`).
- RGB Euclidean distance ranges from `0` to approximately `441.67`.
- Region captures are snapshots. A captured `PixelImage` owns its `pixels`
  buffer and must be released using `pixelImageDestroy()`.

## Important API notes

- Initialize result arrays to zero before their first use, for example:
  `PixelPointArray matches = {0};`. Release them with
  `pixelPointArrayDestroy()`.
- `colourFinderSearch()` and `colourRangeFinderSearch*()` replace any previous
  contents of the supplied, zero-initialized `PixelPointArray`.
- Initialize `PixelColourClusterArray` as `{0}` and release it with
  `colourClusterDetectorFree()`.
- `colourFrequencyAnalyzerAnalyze()` allocates its result array. The caller
  must release it with `free()`.
- `multiPixelTrackerInitialize()` copies the supplied `POINT` coordinate array;
  release the tracker with `multiPixelTrackerDestroy()`.
- `pixelHistoryRecorderInitialize()` and `pixelTrackerInitialize()` initialize
  their state structures. Use the corresponding `Destroy()` function for the
  history recorder and multi-pixel tracker when finished.
- Update functions are non-blocking. Call them from the host application's
  update loop and inspect their return values to determine when fresh samples
  are available. `colourChangeDetectorUpdate()` returns TRUE when a sample is
  captured and writes the threshold result through its `changed` output.
  `multiPixelTrackerUpdate()` returns TRUE when an update pass ran; inspect each
  point's `valid` member for its individual capture status.
- `colourProfileTesterInspect()` reports the active ICC profile exposed by the
  monitor's GDI display device. Capture one report per monitor, then use
  `colourProfileTesterCompare()` to compare their sampled colours, detection
  errors, ICC profile names and bit depths. It does not change Windows colour
  management or guarantee that an application's rendering pipeline uses that
  profile.

## Example: Pixel inspection

```c
#include "pixelInspector.h"

PixelInspectorData data;
if (pixelInspectorGet(260, 123, &data)) {
    // data.rgb, data.hex, data.hsvHue, data.hslLightness, etc
}
```

## Example: Interval-based tracking

```c
#include "pixelTracker.h"

PixelTracker tracker;
if (pixelTrackerInitialize(&tracker, 260, 123, 100)) {
    // Inside the Raylib update loop
    PixelColor color;
    ULONGLONG timestampMs;

    if (pixelTrackerUpdate(&tracker, &color, &timestampMs)) {
        // A fresh sample is available
    }
}
```

## Example: Searching for a colour

```c
#include "colourFinder.h"

PixelPointArray matches = {0};
PixelColor target = { 40, 180, 90 };

if (colourFinderSearch(0, 0, 1920, 1080, target, 5.0, &matches)) {
    // matches.items[0..matches.count-1] contains desktop coordinates
}

pixelPointArrayDestroy(&matches);
```

## Build requirements

- Windows 10 or later recommended.
- C11 compiler (MSVC, MinGW-w64 GCC or Clang targeting Windows).
- Windows GDI and User32 APIs.

For MinGW-w64, compile each source file into an object file:

```powershell
gcc -Wall -Wextra -std=c11 -c PixelTools\pixelToolsCommon.c
# Compile the remaining PixelTools\*.c files in the same way
```

When linking an application that uses these modules, link `gdi32` and `user32`.
The colour conversion and colour-difference modules use the C math library;
MinGW-w64 builds should also link `-lm` where required. MSVC uses the standard
runtime math functions.

## Limitations

- The modules capture the desktop through GDI `BitBlt`. Protected content,
  hardware overlays, secure desktops, and some exclusive-fullscreen surfaces
  may not be capturable through this path.
- Large full-screen searches can allocate substantial memory and take time.
  Prefer a bounded region when possible.
- Pixel sampling reports the colour exposed by the desktop capture path; it is
  not a calibrated measurement of emitted panel light.
- The cluster detector uses 4-neighbour connectivity and groups pixels by
  distance from one supplied target colour. It is not a general-purpose
  perceptual segmentation algorithm.