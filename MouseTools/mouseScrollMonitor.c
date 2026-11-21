#define WIN32_LEAN_AND_MEAN
#include "mouseScrollMonitor.h"

void mouseScrollMonitorInitialize(MouseScrollMonitor *monitor) {
    if (monitor != NULL) {
        ZeroMemory(monitor, sizeof(*monitor));
    }
}

BOOL mouseScrollMonitorHandleMessage(MouseScrollMonitor *monitor,
                                     UINT message, WPARAM wParam) {
    if (monitor == NULL) {
        return FALSE;
    }

    SHORT delta = (SHORT)HIWORD(wParam);
    if (message == WM_MOUSEWHEEL) {
        monitor->lastVerticalDelta = delta;
        monitor->totalVerticalDelta += delta;
        monitor->verticalEventCount++;
    } else if (message == WM_MOUSEHWHEEL) {
        monitor->lastHorizontalDelta = delta;
        monitor->totalHorizontalDelta += delta;
        monitor->horizontalEventCount++;
    } else {
        return FALSE;
    }

    monitor->lastEventTimeMs = GetTickCount64();
    return TRUE;
}

void mouseScrollMonitorReset(MouseScrollMonitor *monitor) {
    mouseScrollMonitorInitialize(monitor);
}
