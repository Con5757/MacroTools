# Keyboard Tools

A collection of independent Windows keyboard utilities written in C. Each tool is a reusable module and deliberately contains no `main()` function. The intended host is a larger application (eventually the MacroTools Raylib GUI).

## Structure

- `keyStateMonitor.c/.h`: Poll the currently held state of all virtual keys.
- `keyRecorder.c/.h`: Record global key-down/key-up events, scan codes, flags, and timestamps.
- `keyPlayback.c/.h`: Replay recorded keyboard events on a non-blocking update loop.
- `keyTester.c/.h`: Test one virtual key and detect press/release transitions.
- `keyCombinationTester.c/.h`: Test whether a configured set of keys is held together.
- `hotkeyMonitor.c/.h`: Register configurable system-wide hotkeys and decode `WM_HOTKEY` messages.
- `keyHoldDurationTracker.c/.h`: Measure held durations for all virtual keys using polling.
- `keyRepeatAnalyzer.c/.h`: Count repeated key-down events and calculate repeat intervals/rates.
- `keyboardLayoutInspector.c/.h`: Inspect the active layout and map virtual keys to scan codes and translated characters.
- `scanCodeInspector.c/.h`: Capture low-level keyboard scan codes and event flags.
- `keyboardInputSimulator.c/.h`: Send virtual-key, scan-code, and Unicode text input.
- `keyboardStateSnapshot.c/.h`: Capture a one-time snapshot of global key-down state.
- `keyboardToolsCommon.h`: Shared event structure used by recording and playback.

## Requirements

- Windows 10 or newer recommended.
- C11 compiler (MinGW GCC or Clang targeting Windows).
- Link against `user32`.

Example object compilation:

```powershell
gcc -Wall -Wextra -std=c11 -c KeyboardTools/keyStateMonitor.c -o keyStateMonitor.o
```

To compile/link a program using several modules, include the relevant `.c` files and link `-luser32`.

## Integration notes

### Normal polling modules

Call `keyStateMonitorUpdate`, `keyTesterUpdate`, `keyCombinationTesterUpdate`, `keyHoldDurationTrackerUpdate`, and `keyboardStateSnapshotCapture` from your application. They do not create threads, sleep, print to the console, or define an entry point.

### Low-level hook modules

`keyRecorder`, `keyRepeatAnalyzer`, and `scanCodeInspector` install `WH_KEYBOARD_LL` hooks. Start and stop them on a thread that continues pumping its Windows message queue. A Raylib host should keep its normal Windows message processing active while a hook is installed. Always call each module's `Stop` function before destroying its state or exiting.

Only one active instance of each low-level-hook module is supported per process by these implementations. Different hook modules can be active at the same time.

### Global hotkeys

`hotkeyMonitorRegister` registers a system-wide hotkey for the calling thread. The host must pass matching `WM_HOTKEY` messages to `hotkeyMonitorHandleMessage`. Call `hotkeyMonitorUnregister` when finished. Hotkey registration can fail when another application already owns the same combination or Windows reserves it.

### Playback and simulated input

Playback and simulator functions send input to the current foreground application. `keyPlaybackUpdate` is non-blocking and should be called regularly. Use `keyPlaybackStop` and `keyPlaybackDestroy` to release any keys held by an incomplete recording. Test input playback with a harmless target before using it in important applications.

### Keyboard layout

Layout and character translation are based on the calling thread's active keyboard layout. The layout may differ between threads or change while the application is running; recapture it when needed.

## Notes on timing and state

- `GetAsyncKeyState` is polled state, so hold-duration accuracy depends on how frequently the host calls the update function. Very brief taps can occur between polls.
- Low-level hook timestamps use `GetTickCount64` milliseconds. They are suitable for relative timing, not wall-clock dates.
- `KeyPlayback` copies its input events, so the recorder can be cleared or destroyed after `keyPlaybackLoadEvents` succeeds.
- The recorder and low-level inspectors observe injected as well as physical events. `KeyRepeatAnalyzer` can optionally exclude injected events.
- `ScanCodeInspector` reports Windows low-level-hook scan codes and flags; it is not a USB bus or raw electrical-signal analyzer.