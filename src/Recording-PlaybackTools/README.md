# Macro Recording & Playback Tools

A modular C11 library containing the 21 Macro Recording & Playback tools. Every tool has its own `.c` and `.h` pair. There is no `main()` function and no console UI. The library is designed to be called by a host application (including a future Raylib GUI).

## Files

- `macroToolsCommon.c/.h`: shared macro step/sequence types, dynamic sequence storage, variables, condition primitives and playback state.
- `macroRecorder.c/.h`: records input actions supplied by the host.
- `macroPlayer.c/.h`: non-blocking timed playback through a caller-provided dispatch callback.
- `macroEditor.c/.h`: insert, remove, move and duplicate steps.
- `macroTimelineViewer.c/.h`: builds chronological timeline entries.
- `macroStepInspector.c/.h`: reads an individual step.
- `macroStepEditor.c/.h`: edits step properties.
- `macroLoopController.c/.h`: configures repeat count.
- `macroScheduler.c/.h`: schedules a sequence for a specified monotonic timestamp.
- `macroSequenceManager.c/.h`: indexes caller-owned sequences.
- `macroConditionManager.c/.h`: evaluates condition steps against runtime variables.
- `macroBranchingEngine.c/.h`: resolves jump and conditional branch destinations.
- `macroVariableManager.c/.h`: stores numeric runtime variables.
- `macroParameterManager.c/.h`: stores declared reusable numeric parameters and overrides.
- `macroCheckpointManager.c/.h`: saves/restores playback cursor state.
- `macroPauseResumeController.c/.h`: pause/resume helpers.
- `macroStopController.c/.h`: safe stop request helpers.
- `macroStepDebugger.c/.h`: single-step and continue controls.
- `macroSpeedController.c/.h`: playback speed multiplier.
- `macroRandomizer.c/.h`: seeded variation for mouse positions and delays.
- `macroImporterExporter.c/.h`: versioned text-based macro interchange.
- `macroValidator.c/.h`: validates macro structure and branch targets.

## Core data model

`MacroSequence` owns a dynamically allocated array of `MacroStep` values. Initialize it with `macroSequenceInitialize()` and release it with `macroSequenceDestroy()`. A step's `delay_ms` is the delay **before** that step. `macroRecorderRecord()` computes this value from the supplied event timestamps.

Step types cover key down/up, mouse movement, mouse button down/up, scroll, wait, variable assignment, condition, jump and checkpoint. The host application's input monitor should translate its events into `MacroStep` values and pass them to the recorder.

Condition steps use:

- `text`: numeric variable name
- `code`: `MacroConditionOperator`
- `number`: comparison value
- `target_step`: true destination
- `alternate_step`: false destination (use `MACRO_NO_TARGET` to continue to the next step)

Jump steps use `target_step`. Branch destinations are zero-based sequence indices.

## Playback integration

Playback is deliberately callback-based. The player does not install hooks or inject inputs by itself. Supply a `MacroDispatchFunction` that converts a macro step into your existing keyboard/mouse input simulation API. This keeps the macro engine testable and avoids coupling it to a specific input backend.

The host should call `macroPlayerUpdate()` from its update loop and pass a monotonic millisecond clock (for example, `GetTickCount64()` on Windows). Limit `max_steps_per_update` to avoid a long macro branch monopolizing a GUI frame. Callback `false` stops playback with `MACRO_PLAYER_ERROR`.

The dispatch callback receives key/mouse actions, variable-assignment steps and checkpoint steps; it should route those to the existing input simulation, variable and checkpoint facilities. `WAIT` steps are timing-only. Condition steps are evaluated through the optional `MacroConditionFunction` installed by `macroPlayerSetConditionHandler()`. The condition callback can call `macroConditionManagerEvaluate()` with a `MacroVariableStore`. The standalone `macroBranchingResolve()` offers the same branch logic for tools that need to inspect/control program flow separately.

`loops_total` is total plays, not extra repeats: `1` means one play, `2` means play twice, and `0` means repeat indefinitely. Use `macroLoopControllerConfigure()` before starting playback.

## Timing rules

All timestamps passed to the recorder, scheduler and playback controller must come from the **same monotonic clock** and use milliseconds. Do not use wall-clock time (calendar time) for elapsed playback timing. `macroPlayerUpdate()` is non-blocking; it never calls `Sleep()`.

Playback speed is a multiplier from `0.05` through `20.0`. A multiplier of `2.0` runs delays at twice normal speed; `0.5` runs at half speed.

## Ownership

- `MacroSequence` owns its `steps` allocation.
- `MacroSequenceManager` stores **non-owning pointers**. Destroying the manager does not destroy the registered sequences.
- `MacroPlaybackState.sequence` is non-owning and must remain valid throughout playback.
- Recorder holds a non-owning pointer to the target sequence while recording.

## Import/export format

The importer/exporter uses a versioned UTF-8 text format (`MACROTOOLS 1`). Names and step text are hex encoded so delimiters and newlines cannot corrupt records. Import parses into a temporary sequence and replaces the destination only after the whole file has been accepted. The importer limits files to 1,000,000 steps.

## Build

The core library is standard C11 and has no Windows-specific dependency. Example with GCC from this directory:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -c *.c
```

Link the resulting object files into your host application. `macroToolsCommon.c` must be included. Some toolchains also require linking the math library (`-lm`). Some tool `.c` files call shared helpers from other modules, so compile/link the complete set together unless you deliberately select a subset and include its dependencies.

## Important scope note

This package manages macro data, sequencing and playback scheduling. It does not install low-level keyboard/mouse hooks or synthesize OS input directly. The GUI/input layer should feed recorded events into `MacroRecorder` and implement the player's dispatch callback using your existing Input Event/Mouse/Keyboard simulation modules. This separation is intentional for future Raylib integration.