#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "mouseButtonMonitor.h"
#include <Windows.h>

BOOL mouseInputSimulatorMoveTo(POINT position);
BOOL mouseInputSimulatorMoveRelative(LONG deltaX, LONG deltaY);
BOOL mouseInputSimulatorSetButton(MouseButton button, BOOL down);
BOOL mouseInputSimulatorScrollVertical(SHORT wheelDelta);
BOOL mouseInputSimulatorScrollHorizontal(SHORT wheelDelta);