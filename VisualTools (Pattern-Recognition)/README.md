# Visual Recognition & Analysis Tools

A collection of independent Windows C modules for screen-region inspection and visual recognition. Every listed tool has its own `.c` and `.h` pair. There are no `main()` functions.

## Image conventions

`VTImage` stores top-down, packed RGB24 pixels. Its `stride` is measured in bytes. All rectangles passed to analysis functions are **image-local** coordinates, with `(0,0)` at the top-left of the supplied `VTImage`. `vtImageCaptureScreen()` accepts virtual-desktop screen coordinates, so a monitor located left of or above the primary monitor can have negative screen coordinates. Capture outputs top-down RGB data.

The caller owns every `VTImage` it creates and must release it with `vtImageDestroy()`. Initialize output images as `VTImage image = {0};`. Functions that create or replace image outputs expect a zero-initialized or library-created destination and release its prior contents when replacing it. Do not pass an uninitialized stack variable as an image destination. `vtImageCrop()` does not support using the same object as both source and destination.

## Modules

| Module | Main API | Purpose |
|---|---|---|
| Region Scanner | `vtRegionScan` | Counts pixels matching RGB tolerance or brightness bounds and returns their bounding box and coverage. |
| Region Inspector | `vtRegionInspect` | Reports region dimensions, pixel count, average/minimum/maximum RGB and average brightness. |
| Pattern Matcher | `vtPatternFind` | Searches for a sparse pattern defined by relative sample points and per-point colour tolerances. |
| Template Matcher | `vtTemplateMatch` | Finds the best matching location for a supplied RGB template. |
| Image Recognition Tool | `vtImageRecognize` | Classifies an image region against a set of named templates. |
| Image Similarity Analyzer | `vtImageCompare` | Calculates mean absolute error, RMSE and normalized similarity for equal-sized images. |
| Shape Detector | `vtDetectShapes` | Finds connected foreground components and estimates rectangle, ellipse, line or blob classes. |
| Edge Detector | `vtDetectEdges` | Produces a Sobel edge mask and counts detected edge pixels. |
| Motion Detector | `vtDetectMotion` | Compares consecutive equal-sized frames and reports changed-pixel ratio and bounds. |
| Object Position Tracker | `vtObjectTrackerInitialize`, `vtObjectTrackerUpdate` | Tracks a supplied image template, narrowing subsequent searches around the previous position. |
| Text Recognition Tool (OCR) | `vtTextRecognize` | Adapter point for an OCR engine supplied by the host application. |
| UI Element Detector | `vtDetectUIElements` | Heuristic detector for button-, icon- and panel-like foreground shapes. |
| Template Library Manager | `vtTemplateLibrary*` | Adds, retrieves, removes and persists named image templates. |
| Recognition Accuracy Tester | `vtRecognitionMeasureAccuracy` | Calculates TP/FP/FN, precision, recall and F1 from predicted and ground-truth rectangles using IoU. |
| Multi-Target Detector | `vtDetectMultipleTargets` | Searches multiple templates in one image and returns matches that meet their thresholds. |
| Region Occupancy Detector | `vtRegionMeasureOccupancy` | Measures changed-pixel occupancy against a baseline within a region. |

## OCR integration

OCR is deliberately exposed as a callback (`VTOcrEngine`). The core module validates the image and region, then invokes the engine supplied by the application. This keeps the library in C and avoids claiming a built-in OCR engine where none is linked. The host can adapt Windows OCR, Tesseract, or another engine to this callback. When no engine is provided, `vtTextRecognize()` returns `VT_STATUS_UNSUPPORTED`. The callback must return UTF-8 text and must not write beyond the provided buffer capacity.

## Dependencies

- Windows SDK (`Windows.h`, GDI)
- C11 compiler
- Link `user32` and `gdi32` for `visualToolsCommon.c` screen capture
- Link `m` only on toolchains/platforms where math functions are not provided by the C runtime (for `imageSimilarityAnalyzer.c`)

Most analysis modules are portable C once a `VTImage` is supplied. Only the common screen-capture implementation depends on the Windows GDI API.

## Example compilation

Compile the full module set as object files with MinGW GCC:

```powershell
$sources = Get-ChildItem DisplayTools/visualRecognition -Filter *.c | ForEach-Object { $_.FullName }
gcc -Wall -Wextra -std=c11 -c $sources
```

To link a Windows executable, include `-luser32 -lgdi32` and the rest of your application. There is intentionally no executable entry point in this module set.

## Raylib integration pattern

Capture or receive a `VTImage`, call the relevant analysis function during your application update, and use the returned structures to update Raylib state. These functions do not create a window, own the Raylib loop, or print to the console. Keep capture and analysis off the render-critical path if you need high frame rates; the matching and connected-component routines are CPU-based and their cost depends on image/template sizes.

## Limitations to understand

- Template matching uses normalized per-channel absolute RGB error. It is not scale-, rotation-, or illumination-invariant.
- Pattern matching checks sampled colour points, not feature descriptors.
- Shape and UI-element detection use simple brightness-threshold connected components. They are heuristics, not a trained vision model or full UI Automation provider.
- Motion detection reports the aggregate changed area; it does not segment multiple moving objects.
- Multi-target detection returns at most one best match per template definition.
- The template library file is a simple binary format intended for this application, not a general image format. Call `vtTemplateLibraryInitialize()` before loading into a library, and call `vtTemplateLibraryDestroy()` when finished.
- `vtObjectTrackerInitialize()` stores a non-owning pointer to the template; keep that image alive until the tracker is no longer used.
- Screen capture can be blocked or blank for protected content, secure desktops, elevated windows, or some hardware-accelerated surfaces.

## Status

Source files are organized as independent `.c` / `.h` modules and are intended for integration into MacroTools. Build and runtime verification should be performed on Windows with the target Windows SDK and compiler. The OCR callback requires an external OCR backend.
