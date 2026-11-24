# Input Event Management

Windows C11 input-event utilities intended for integration into a larger application (including a future Raylib GUI). Each listed tool has an independent `.c` / `.h` pair. There are no `main()` functions.

## Contents

- Input Event Logger
- Input Event Monitor
- Input Event Recorder
- Input Event Playback
- Input Event Filter
- Input Event Comparator
- Input State Inspector
- Input Sequence Validator
- Input Event Timestamp Analyzer
- Input Device Inspector
- Input Device Monitor
- Input Conflict Detector
- Input Simulation Tester
- Input Source Identifier

Shared files: `inputEventCommon.c` and `inputEventCommon.h` define the unified event structure and dynamic event buffer.

## Important integration notes

### Raw Input requires a host window procedure

`inputEventMonitorInitialize()` registers the host window for Raw Input. Call `inputEventMonitorHandleMessage()` from that window's `WndProc` for `WM_INPUT` and `WM_INPUT_DEVICE_CHANGE`, then forward the returned `InputEvent` records to the logger/recorder/filter as needed. The host must continue dispatching Windows messages. Do not register Raw Input twice for the same usage page/usage from competing components; prefer one central registration and share its device-change events.

Example shape:

```c
case WM_INPUT:
case WM_INPUT_DEVICE_CHANGE: {
    InputEvent events[INPUT_EVENT_MONITOR_MAX_EVENTS];
    size_t count = inputEventMonitorHandleMessage(
        &monitor, message, wParam, lParam, events,
        INPUT_EVENT_MONITOR_MAX_EVENTS);
    for (size_t i = 0; i < count; ++i) {
        inputEventLoggerRecord(&logger, &events[i]);
        inputEventRecorderAppend(&recorder, &events[i]);
    }
    // For foreground WM_INPUT -> always allow Windows to perform cleanup
    if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
        return DefWindowProc(hwnd, message, wParam, lParam);
    break;
}
```

### Playback and simulation

`inputEventPlaybackUpdate()` is non-blocking and should be called repeatedly by the application's update loop. It sends events using `SendInput`; it does not move the cursor to the original absolute screen location for ordinary relative raw-mouse deltas. Device notifications are skipped. `InputEventPlayback` keeps a pointer to the caller's event array, so that array must remain valid until playback finishes or is stopped.

`inputSimulationTester*()` uses `SendInput` and affects the real desktop. Use it only when an intentional test action is desired. It reports how many inputs Windows accepted, not whether the target application reacted to them.

### Physical vs injected source identification

Raw Input events normally carry a device handle, allowing the tools to label them as device-backed/physical. A missing handle is classified as unknown rather than guessed. Low-level keyboard/mouse hook flags (`LLKHF_INJECTED`, `LLMHF_INJECTED` and lower-integrity variants) can identify injected events; call the corresponding `inputSourceIdentifierApply*HookFlags()` function from a low-level hook callback if your host app installs those hooks. This package does not install global hooks automatically. Some input paths cannot be reliably attributed to a physical device or a specific process using these APIs alone.

### State inspection

`inputStateInspectorCapture()` polls the current asynchronous virtual-key and mouse-button states. It is a snapshot, not an event history, and may not identify which physical device generated the state.

### Device enumeration

`inputDeviceInspectorEnumerate()` queries Raw Input devices. It reports the required device count through `*count`; if the supplied capacity is too small it returns `FALSE` after filling the available prefix.

## Compilation

Compile the required `.c` files together with your application. For example, for a program using the monitor, logger, and recorder:

```powershell
gcc -Wall -Wextra -std=c11 inputEventCommon.c inputEventMonitor.c inputEventLogger.c inputEventRecorder.c yourApp.c -o yourApp.exe -luser32 -lm
```

Link Windows APIs as appropriate; typical dependencies include `user32`. The source uses Windows SDK Raw Input, `GetAsyncKeyState`, and `SendInput` declarations. Build with a Windows compiler/SDK (MSVC, MinGW-w64 GCC, or Clang targeting Windows).

## Ownership and lifecycle

- Initialize every stateful struct before use.
- Call the corresponding `Destroy` / `Shutdown` function when finished.
- Event arrays returned by recorder accessors are owned by the recorder and become invalid if the recorder is modified or destroyed.
- Event buffers grow dynamically; check append return values for allocation failure.
- The sequence validator allows repeated key-down events by default because keyboard auto-repeat can produce them; use `inputSequenceValidatorValidateWithOptions()` to disable repeats or relax the requirement that all inputs be released at the end.
- The event timestamp is `GetTickCount64()` milliseconds. It is suitable for ordering and elapsed-time analysis during a boot session, not as a wall-clock date/time.

## Scope

These are reusable utility modules, not a complete input driver. Raw Input sees device-level input delivered to the registered window; low-level hooks are a separate Windows mechanism and require a message loop. The utilities do not install keyboard or mouse hooks, bypass desktop security, or guarantee delivery while another application is in a secure desktop.