#define WIN32_LEAN_AND_MEAN
#include "mouseClickLogger.h"

#include <stdint.h>
#include <stdlib.h>

#define MOUSE_CLICK_LOGGER_INITIAL_CAPACITY 64u

static BOOL mouseClickLoggerReserve(MouseClickLogger *logger, size_t required) {
  if (required <= logger->eventCapacity) {
    return TRUE;
  }

  size_t capacity = logger->eventCapacity ? logger->eventCapacity
                                          : MOUSE_CLICK_LOGGER_INITIAL_CAPACITY;
  while (capacity < required) {
    if (capacity > SIZE_MAX / 2) {
      capacity = required;
      break;
    }
    capacity *= 2;
  }

  if (capacity > SIZE_MAX / sizeof(*logger->events)) {
    return FALSE;
  }

  MouseClickEvent *events =
      (MouseClickEvent *)realloc(logger->events, capacity * sizeof(*events));
  if (events == NULL) {
    return FALSE;
  }

  logger->events = events;
  logger->eventCapacity = capacity;
  return TRUE;
}

BOOL mouseClickLoggerInitialize(MouseClickLogger *logger) {
  if (logger == NULL) {
    return FALSE;
  }
  ZeroMemory(logger, sizeof(*logger));
  return TRUE;
}

BOOL mouseClickLoggerStart(MouseClickLogger *logger) {
  if (logger == NULL) {
    return FALSE;
  }

  logger->eventCount = 0;
  logger->previousButtonState = mouseButtonMonitorGetState();
  logger->startTimeMs = GetTickCount64();
  logger->isLogging = TRUE;
  return TRUE;
}

MouseClickLoggerUpdateResult mouseClickLoggerUpdate(MouseClickLogger *logger) {
  if (logger == NULL) {
    return MOUSE_CLICK_LOGGER_ERROR;
  }
  if (!logger->isLogging) {
    return MOUSE_CLICK_LOGGER_NO_EVENT;
  }

  DWORD currentState = mouseButtonMonitorGetState();
  DWORD changed = currentState ^ logger->previousButtonState;
  if (changed == 0) {
    return MOUSE_CLICK_LOGGER_NO_EVENT;
  }

  POINT position;
  if (!GetCursorPos(&position)) {
    return MOUSE_CLICK_LOGGER_ERROR;
  }

  ULONGLONG timestamp = GetTickCount64() - logger->startTimeMs;
  BOOL recorded = FALSE;

  for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
    DWORD mask = (DWORD)(1u << i);
    if ((changed & mask) == 0) {
      continue;
    }

    if (logger->eventCount == SIZE_MAX ||
        !mouseClickLoggerReserve(logger, logger->eventCount + 1)) {
      return MOUSE_CLICK_LOGGER_ERROR;
    }

    MouseClickEvent *event = &logger->events[logger->eventCount++];
    event->button = (MouseButton)i;
    event->type = (currentState & mask) ? MOUSE_CLICK_EVENT_PRESSED
                                        : MOUSE_CLICK_EVENT_RELEASED;
    event->position = position;
    event->timestampMs = timestamp;
    recorded = TRUE;
  }

  logger->previousButtonState = currentState;
  return recorded ? MOUSE_CLICK_LOGGER_EVENT_RECORDED
                  : MOUSE_CLICK_LOGGER_NO_EVENT;
}

void mouseClickLoggerStop(MouseClickLogger *logger) {
  if (logger != NULL) {
    logger->isLogging = FALSE;
  }
}

void mouseClickLoggerClear(MouseClickLogger *logger) {
  if (logger != NULL) {
    logger->eventCount = 0;
    logger->previousButtonState = mouseButtonMonitorGetState();
  }
}

void mouseClickLoggerDestroy(MouseClickLogger *logger) {
  if (logger == NULL) {
    return;
  }
  free(logger->events);
  ZeroMemory(logger, sizeof(*logger));
}

const MouseClickEvent *mouseClickLoggerGetEvents(const MouseClickLogger *logger,
                                                 size_t *eventCount) {
  if (eventCount != NULL) {
    *eventCount = logger != NULL ? logger->eventCount : 0;
  }
  return logger != NULL ? logger->events : NULL;
}
