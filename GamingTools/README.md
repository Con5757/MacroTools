# Game-Specific Utilities

A collection of independent C11 modules for external game observation, UI inspection,
resolution calibration and session logging. The modules are designed to be called by a
larger application (including a future Raylib GUI) and do not define `main()`.

## Contents

| Tool | Files | Purpose |
|---|---|---|
| Game Window Detector | `gameWindowDetector.c/.h` | Enumerate visible top-level windows by title substring on Windows. |
| Game Resolution Detector | `gameResolutionDetector.c/.h` | Read a window's client/rendering dimensions. |
| Game State Detector | `gameStateDetector.c/.h` | Identify predefined visual states using reference images. |
| Menu Detector | `gameMenuDetector.c/.h` | Locate a supplied menu template in a frame. |
| Loading Screen Detector | `loadingScreenDetector.c/.h` | Heuristic for sustained frame-to-frame visual activity. |
| HUD Inspector | `hudInspector.c/.h` | Measure colour and brightness statistics in a HUD region. |
| Health Bar Detector | `healthBarDetector.c/.h` | Estimate fill ratio by matching a known fill colour. |
| Progress Bar Tracker | `progressBarTracker.c/.h` | Track fill ratio in a defined region. |
| Cooldown Tracker | `cooldownTracker.c/.h` | Estimate readiness from a known ready-state colour. |
| Inventory Grid Inspector | `inventoryGridInspector.c/.h` | Divide an inventory region into cells and estimate occupancy. |
| Game Timer Reader | `gameTimerReader.c/.h` | Parse `MM:SS` or `HH:MM:SS` via an OCR callback. |
| Game Notification Detector | `gameNotificationDetector.c/.h` | Detect changes within a specified notification region. |
| Game Text Reader | `gameTextReader.c/.h` | Adapt a caller-provided OCR implementation. |
| Game State Transition Monitor | `gameStateTransitionMonitor.c/.h` | Debounce identified state changes across consecutive frames. |
| Game Resolution Calibration | `gameResolutionCalibration.c/.h` | Fit and apply a least-squares 2D affine transform. |
| Game UI Template Manager | `gameUITemplateManager.c/.h` | Save and load portable RGB24 reference templates. |
| Game Session Logger | `gameSessionLogger.c/.h` | Write timestamped session events as CSV. |

## Shared image format

`gameToolsCommon.h` defines `GTImage`, a **non-owning** RGB24 image view. Pixels are
row-major, top-down, with three bytes per pixel in R, G, B order. `stride` is the number
of bytes between row starts and may exceed `width * 3`. The caller owns the image memory
and must keep it alive for the duration of every call that uses the image.

`GTRect` uses pixel coordinates and positive width/height. A rectangle is valid only
when the entire region lies inside the image. The image helper functions do not capture
the screen; pass frames obtained from your existing screen-capture modules.

## Integration notes

- **Window detection:** `gameWindowDetector` and `gameResolutionDetector` use the Windows
  API when compiled on Windows. On non-Windows platforms their functions return `false`.
  The window detector enumerates visible top-level windows; matching a title does not
  prove that a window belongs to a game.
- **Visual classifiers:** State and menu detection use reference-image matching. Results
  depend on resolution, animation, scaling, overlays and capture consistency. Choose
  templates from the same UI scale and define tolerances through testing.
- **Loading detection:** The loading-screen detector is intentionally heuristic. It
  reports sustained image activity; gameplay animation can also satisfy that condition.
  It is not a universal loading-screen recognizer.
- **Bars and cooldowns:** These detectors use supplied reference colours and rectangular
  regions. They do not infer arbitrary colours or shapes. Calibrate regions/colours for
  each supported game and UI theme.
- **OCR:** No OCR engine is bundled. `gameTextReader` and `gameTimerReader` accept a callback
  that can wrap an installed OCR engine. The timer parser accepts `MM:SS` and `HH:MM:SS`.
- **Template files:** The template manager writes a small binary RGB24 format (`GTMPL01`)
  using the host's integer byte order. It is intended for the same application/platform,
  not as a cross-endian interchange format. Load into a zero-initialized or otherwise
  released `GTGameUITemplate`; call `gameUITemplateManagerRelease` when finished.
- **Memory ownership:** Any `GTImage` supplied to a function is borrowed. The loading and
  notification detector structs own their internal previous-frame buffers; initialize
  them before use and call their matching destroy function when done.
- **Threading:** These utilities do not create threads. Call them from the host's update
  loop or a worker thread managed by the application. Do not mutate an image buffer while
  a function is reading it.

## Build

All modules use C11. On Windows, compile and link the `.c` files into your application.
The window modules use `user32`; other modules use the C runtime and `gameToolsCommon.c`.

Example MinGW GCC compile of all modules into object files:

```powershell
$files = Get-ChildItem . -Filter *.c | ForEach-Object { $_.FullName }
foreach ($file in $files) {
    gcc -Wall -Wextra -Werror -std=c11 -c $file
}
```

Link the application's final executable with `-luser32` on Windows when using the
window detector. When linking all modules, include `gameToolsCommon.c` exactly once.

## Disclaimer

These tools are intended for legitimate external automation, observation and testing purposes. They must not be used to cheat, bypass anti-cheat systems, circumvent security measures, gain unauthorized access or interfere with applications. Users are responsible for ensuring their use complies with the applicable terms, policies and laws governing the software they interact with.