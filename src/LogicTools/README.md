# Logic & Automation Utilities

A collection of reusable, standalone C11 utilities for automation logic. Each tool is provided as its own `.c` / `.h` pair. The modules have no `main()` functions and do not create their own threads, timers, windows or event loops; the host application drives them.

## Modules

| Tool | Files | Purpose |
|---|---|---|
| Condition Evaluator | `conditionEvaluator.c/.h` | Numeric comparisons with configurable equality tolerance, plus boolean AND/OR/NOT helpers. |
| State Machine Tester | `stateMachineTester.c/.h` | Holds a current state and applies the first transition whose condition callback succeeds. |
| Timer Manager | `timerManager.c/.h` | Manages up to 64 independently identified elapsed/duration timers. |
| Counter Manager | `counterManager.c/.h` | Stores named-by-ID signed 64-bit counters with overflow-safe addition. |
| Variable Inspector | `variableInspector.c/.h` | Stores up to 64 numeric or boolean runtime variables and exposes a resolver adapter. |
| Expression Evaluator | `expressionEvaluator.c/.h` | Evaluates arithmetic expressions, parentheses, variables, `pi` and `e`. |
| Sequence Generator | `sequenceGenerator.c/.h` | Generates finite arithmetic sequences. |
| Random Number Generator | `randomNumberGenerator.c/.h` | Seedable xorshift64* random generator with integer and floating-point ranges. |
| Threshold Monitor | `thresholdMonitor.c/.h` | Reports when a value crosses into or out of a configured threshold side. |
| Event Trigger Manager | `eventTriggerManager.c/.h` | Subscribes callbacks to event IDs and dispatches events. |
| Wait Condition Tester | `waitConditionTester.c/.h` | Non-blocking condition polling with a timeout and configurable poll interval. |
| Timeout Manager | `timeoutManager.c/.h` | Tracks independent operation deadlines and remaining time. |
| Retry Manager | `retryManager.c/.h` | Non-blocking retry state machine with attempt limits and retry delay. |
| Debounce Utility | `debounceUtility.c/.h` | Rejects repeated events inside a debounce interval. |
| Cooldown Manager | `cooldownManager.c/.h` | Enforces per-action cooldown intervals. |
| Action Queue Manager | `actionQueueManager.c/.h` | Fixed-capacity FIFO queue of callback-based actions. |
| Task Scheduler | `taskScheduler.c/.h` | Cooperative one-shot and repeating task scheduler. |

## Requirements

- C11 compiler (GCC, Clang or MSVC with C11 support)
- Standard C library
- Math library for the expression evaluator (`pow`, `fmod`, `isfinite`, and `fabs`)

There are no Windows API or Raylib dependencies in this category. The caller supplies monotonic time in milliseconds to timing-based modules. Use a monotonic clock such as `GetTickCount64()` on Windows or `QueryPerformanceCounter()` converted to milliseconds; do not pass wall-clock time.

## Compilation

From this directory, compile the modules together with your application source. For GCC or Clang:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic \
    -I. your_application.c *.c -lm -o your_application
```

For MinGW GCC on Windows, the same command works with the appropriate application source and output extension. If you build a static library, compile each `.c` file into an object and archive those objects; link the math library when using the expression evaluator.

## Integration pattern

These utilities are intended to be updated from the host application's main loop. They do not block waiting for conditions or delays.

```c
#include "waitConditionTester.h"
#include "taskScheduler.h"

// Obtain this from a monotonic clock supplied by the host
uint64_t nowMs = getMonotonicMilliseconds();

// Each frame/update:
laWaitUpdate(&waitCondition, nowMs);
laTaskSchedulerUpdate(&scheduler, nowMs, 8);
```

The `getMonotonicMilliseconds()` function above is illustrative and is not part of this library.

## Expression syntax

The expression evaluator supports:

- Numeric literals, including decimal and scientific notation
- Variable identifiers resolved through an `LAValueResolver` callback
- Constants `pi` and `e`
- Parentheses
- Unary `+` and `-`
- Binary `+`, `-`, `*`, `/`, `%`, and `^`

Exponentiation is right-associative. Division or remainder by zero, unknown variables, malformed expressions and non-finite results return an error status. Function calls (for example `sin(x)`) are not currently supported.

Example using the variable store:

```c
LAVariableStore variables;
laVariableStoreInitialize(&variables);
laVariableSetNumber(&variables, "speed", 2.5);

double value;
size_t errorOffset;
LAExpressionStatus status = laExpressionEvaluate(
    "speed * 4 + pi",
    laVariableResolveNumber,
    &variables,
    &value,
    &errorOffset
);
```

## Capacity and behavior notes

- Managers with fixed arrays hold up to `LA_MAX_ITEMS` entries (64 by default). Check each function's return value when adding entries.
- The action queue is FIFO and rejects new entries when full. `laActionQueueProcess()` executes no more than the caller's requested budget.
- The task scheduler is cooperative. Repeating tasks skip missed intervals rather than executing a burst of catch-up callbacks after a long pause.
- Wait conditions, timeouts, retries, timers, debounce and cooldown checks are advanced only when the host calls their update/check functions.
- Event callbacks and queued action callbacks execute synchronously on the thread that dispatches/processes them. Keep callbacks short or hand expensive work to a worker system owned by the application.
- The random number generator is deterministic for a given seed. Seed it explicitly; do not use it as a cryptographic random source.
- State machine transitions are checked in insertion order. The first matching transition is applied per call to `laStateMachineStep()`.

## Files

Each utility has an independent implementation and public header. `logicToolsCommon.c/.h` contains shared value types, callback types, fixed-size name copying and saturating deadline arithmetic.