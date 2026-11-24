#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

typedef struct {
  DWORD virtualKey;
  DWORD scanCode;
  DWORD flags;
  UINT message;
  BOOL keyDown;
  BOOL extended;
  BOOL injected;
  ULONGLONG timestampMs;
} ScanCodeInspectorEvent;

typedef struct {
  HHOOK hook;
  BOOL isRunning;
  BOOL hasEvent;
  ULONGLONG totalEvents;
  ScanCodeInspectorEvent lastEvent;
} ScanCodeInspector;

BOOL scanCodeInspectorInitialize(ScanCodeInspector *inspector);
BOOL scanCodeInspectorStart(ScanCodeInspector *inspector);
void scanCodeInspectorStop(ScanCodeInspector *inspector);
BOOL scanCodeInspectorGetLastEvent(const ScanCodeInspector *inspector,
                                   ScanCodeInspectorEvent *event,
                                   ULONGLONG *totalEvents);