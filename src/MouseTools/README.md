# MacroTools - Remaining Mouse Tools

This archive contains the mouse tools after **Mouse Tracker** and **Mouse Recorder**. Each top-level tool has its own `.c` and `.h` file under `DisplayTools/`. No file defines `main()`.

## Contents

| Tool | Files | Purpose |
|---|---|---|
| Mouse Playback | `mousePlayback.c/.h` | Replays timestamped positions recorded by Mouse Recorder. Supports looping and non-blocking updates. |
| Mouse Button Monitor | `mouseButtonMonitor.c/.h` | Reports the currently held left, right, middle, X1, and X2 buttons as a bitmask. |
| Mouse Click Logger | `mouseClickLogger.c/.h` | Records button press/release events with desktop coordinates and elapsed timestamps. |
| Mouse Click Tester | `mouseClickTester.c/.h` | Reports button-down and button-up transitions and per-button counts. |
| Mouse Scroll Monitor | `mouseScrollMonitor.c/.h` | Receives vertical and horizontal wheel messages and tracks deltas and event counts. |
| Mouse Movement Analyzer | `mouseMovementAnalyzer.c/.h` | Calculates segment distance, direction, elapsed time, and cursor speed. |
| Mouse Distance Tracker | `mouseDistanceTracker.c/.h` | Accumulates the total cursor distance between samples. |
| Mouse Position Sampler | `mousePositionSampler.c/.h` | Stores cursor coordinates at a configurable interval, including repeated positions. |
| Mouse Path Visualizer | `mousePathVisualizer.c/.h` | Calculates recorded-path bounds and maps points into a GUI drawing rectangle. |
| Mouse Sensitivity Analyzer | `mouseSensitivityAnalyzer.c/.h` | Compares Windows Raw Input deltas with cursor-pixel movement. |
| Mouse Input Simulator | `mouseInputSimulator.c/.h` | Sends controlled cursor movement, button, and scroll input through `SendInput`. |
| Mouse Boundary Detector | `mouseBoundaryDetector.c/.h` | Detects entry into and exit from a rectangle and estimates the crossed edge. |
| Mouse Region Monitor | `mouseRegionMonitor.c/.h` | Monitors whether the cursor is inside a specified desktop-coordinate rectangle. |

## Requirements

- Windows
- C11-compatible compiler (MinGW GCC or Clang)
- Windows API
- Link `user32` when building an executable
- Link the math library (`-lm`) when required by your compiler

## Dependencies on existing Mouse Tools

`mousePlayback.c/.h` and `mousePathVisualizer.c/.h` use `MouseRecorderSample` from the existing `DisplayTools/mouseRecorder.h` module. Keep that header available in the same `DisplayTools` directory.

`mouseClickLogger.c` and `mouseClickTester.c` call `mouseButtonMonitorGetState()`, so include and compile `mouseButtonMonitor.c` when using either module.

## GUI integration notes

Most modules expose initialize/start/update/stop/getter functions and do not run their own loops. Call their update functions from the Raylib application's update loop.

Two modules consume native Windows messages:

- **Mouse Scroll Monitor:** forward `WM_MOUSEWHEEL` and `WM_MOUSEHWHEEL` to `mouseScrollMonitorHandleMessage()` from the host window procedure.
- **Mouse Sensitivity Analyzer:** register Raw Input for the Raylib window with `mouseSensitivityAnalyzerRegister()`, then forward `WM_INPUT` to `mouseSensitivityAnalyzerHandleMessage()` from the host window procedure. Call `mouseSensitivityAnalyzerUnregister()` during cleanup.

`Mouse Path Visualizer` deliberately does not depend on Raylib. It provides coordinate bounds and point mapping; Raylib can draw the returned points using its own drawing functions.

`Mouse Input Simulator` sends real system input. Use it only when the application intends to move the system cursor or generate actual mouse input.

## Compilation

Compile these files as object files and link them into the application's existing executable. For example, from `DisplayTools` with MinGW:

```powershell
gcc -Wall -Wextra -std=c11 -c mousePlayback.c mouseButtonMonitor.c mouseClickLogger.c mouseClickTester.c mouseScrollMonitor.c mouseMovementAnalyzer.c mouseDistanceTracker.c mousePositionSampler.c mousePathVisualizer.c mouseSensitivityAnalyzer.c mouseInputSimulator.c mouseBoundaryDetector.c mouseRegionMonitor.c
```

When linking an executable, link the required Windows API and math libraries, for example:

```powershell
gcc *.o -o MacroTools.exe -luser32 -lm
```