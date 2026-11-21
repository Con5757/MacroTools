#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "mouseButtonMonitor.h"
#include <Windows.h>
#include <stddef.h>

typedef enum {
  MOUSE_CLICK_EVENT_RELEASED = 0,
  MOUSE_CLICK_EVENT_PRESSED = 1
} MouseClickEventType;

typedef struct {
  MouseButton button;
  MouseClickEventType type;
  POINT position;
  ULONGLONG timestampMs;
} MouseClickEvent;

typedef struct {
  MouseClickEvent *events;
  size_t eventCount;
  size_t eventCapacity;
  ULONGLONG startTimeMs;
  DWORD previousButtonState;
  BOOL isLogging;
} MouseClickLogger;

typedef enum {
  MOUSE_CLICK_LOGGER_ERROR = -1,
  MOUSE_CLICK_LOGGER_NO_EVENT = 0,
  MOUSE_CLICK_LOGGER_EVENT_RECORDED = 1
} MouseClickLoggerUpdateResult;

BOOL mouseClickLoggerInitialize(MouseClickLogger *logger);
BOOL mouseClickLoggerStart(MouseClickLogger *logger);
MouseClickLoggerUpdateResult mouseClickLoggerUpdate(MouseClickLogger *logger);
void mouseClickLoggerStop(MouseClickLogger *logger);
void mouseClickLoggerClear(MouseClickLogger *logger);
void mouseClickLoggerDestroy(MouseClickLogger *logger);
const MouseClickEvent *mouseClickLoggerGetEvents(const MouseClickLogger *logger,
                                                 size_t *eventCount);
