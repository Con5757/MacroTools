# Timing & Performance Tools

A set of independent Windows C modules for profiling timing, input, process and system performance. Every tool is exposed as a `.c` / `.h` pair. No file defines `main()`.

## Files

- `intervalProfiler.c/.h` : actual sampling interval versus requested interval, mean error and extrema.
- `highResolutionTimer.c/.h` : start/stop elapsed-time measurement using `QueryPerformanceCounter`.
- `executionTimer.c/.h` : repeated operation timing with count, minimum, maximum and average.
- `inputLatencyTester.c/.h` : measure time between an application marker and the caller reporting input detection.
- `pollingRateAnalyzer.c/.h` : event interval and estimated event frequency.
- `frameRateMonitor.c/.h` : instantaneous and average frame rate.
- `timingDriftAnalyzer.c/.h` : signed and absolute difference from a scheduled QPC deadline.
- `jitterAnalyzer.c/.h` : event-interval mean, range and sample standard deviation.
- `sleepAccuracyTester.c/.h` : measure elapsed time around Windows `Sleep`.
- `cpuUsageMonitor.c/.h` : system CPU and current-process CPU utilization.
- `memoryUsageMonitor.c/.h` : current-process working set, private bytes and peak usage.
- `threadPerformanceAnalyzer.c/.h` : CPU time used by a caller-supplied thread handle.
- `systemLoadMonitor.c/.h` : system CPU, physical/page-file memory availability and processor count.
- `performanceBenchmark.c/.h` : repeated elapsed-time benchmark and operations/second.
- `timingToolsCommon.c/.h` : shared QPC, FILETIME and processor-count helpers.

## Requirements

- Windows 10/11 or a compatible Windows version providing the documented Win32 APIs.
- C11 compiler and Windows SDK headers.
- `Psapi.lib` for `GetProcessMemoryInfo` (`-lpsapi` with MinGW).
- `Kernel32.lib` / standard Windows system libraries for the remaining APIs.
- Link the math library where required by the toolchain (`-lm` for MinGW environments that require it).

## Integration

Include the individual headers you need and compile the corresponding `.c` files together with `timingToolsCommon.c`:

```powershell
gcc -Wall -Wextra -std=c11 -c DisplayTools/TimingPerformanceTools/timingToolsCommon.c
```

Example source list for all modules:

```powershell
gcc -Wall -Wextra -std=c11 -c DisplayTools/TimingPerformanceTools/*.c
```

Link your application with `-lpsapi` (and `-lm` if required). Compile modules into object files or add their sources to your application build; do not compile them as standalone executables.

## Raylib usage

Initialize each monitor once, then call its update/record function from the appropriate point in your application loop. Most APIs are non-blocking and return results through caller-owned structs. The `ttSleepAccuracyTesterRun()` function is deliberately blocking because it must call the Windows `Sleep()` API; run it on a worker thread or outside the UI frame loop.

Example:

```c
#include "frameRateMonitor.h"

TTFrameRateMonitor fps;
TTFrameRateResult fpsResult;
ttFrameRateMonitorInitialize(&fps);

// Inside the application's update loop
if (ttFrameRateMonitorUpdate(&fps, &fpsResult) && fpsResult.frameCount > 0) {
    // Display fpsResult.instantaneousFps / fpsResult.averageFps in Raylib
}
```

## Important measurement notes

- `QueryPerformanceCounter` provides high-resolution monotonic timing. Values are meaningful as differences within the same machine/boot session; do not persist raw counter values as wall-clock timestamps.
- All intervals and elapsed durations are milliseconds unless the field name says otherwise.
- Input latency is measured from the moment the application calls `ttInputLatencyTesterMark()` until it calls `ttInputLatencyTesterComplete()`. It does not include hardware-to-Windows latency before the application receives an event.
- Polling and frame-rate results describe the cadence at which the caller invokes the functions. They do not reveal a device's hardware polling rate unless the caller is recording actual device events.
- `processCpuPercent` is normalized against the total capacity of all logical processors (100% means the process is using the equivalent of all logical processors). `processCpuPercentPerCore` is relative to one logical processor and can exceed 100% for a multi-threaded process.
- System CPU percentage is based on deltas from `GetSystemTimes`; on the first `ttSystemLoadMonitorUpdate()` call, `cpuUsageValid` is `FALSE` because it only establishes a baseline. Memory and processor-count fields are still valid.
- `ttThreadPerformanceAnalyzerUpdate()` does not take ownership of or close the provided thread handle. The caller must ensure the handle stays valid between samples and has permission to query it.
- `ttSleepAccuracyTesterRun()` measures elapsed wall time; overshoot is subject to scheduler load, timer resolution, power policy and virtualization.
- Call update functions at sensible intervals. Extremely frequent monitoring can itself affect the performance being measured.

## Naming

The `tt` prefix means **Timing Tools** and is used for shared helper and API names to avoid collisions with other project modules.