#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Dbt.h>
#include <stdio.h>
#include "monitorConfigTracker.h"

static const char windowClass[] = "MonitorConfigTrackerWindow";

static LRESULT CALLBACK monitorConfigCallback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    switch (message) {
        case WM_DISPLAYCHANGE:
            printf("\nDisplay configuration changed.\n");
            printf(
                "Resolution: %lu x %lu\n",
                (DWORD)LOWORD(lParam),
                (DWORD)HIWORD(lParam)
            );
            printf("Bits per pixel: %lu\n", (DWORD)wParam);
            fflush(stdout);
            return 0;

        case WM_DEVICECHANGE:
            if (wParam == DBT_DEVNODES_CHANGED ||
                wParam == DBT_DEVICEARRIVAL ||
                wParam == DBT_DEVICEREMOVECOMPLETE) {
                printf("\nDisplay device configuration changed.\n");
                fflush(stdout);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcA(hwnd, message, wParam, lParam);
}

void monitorConfigTracker(void) {
    HINSTANCE instance = GetModuleHandleA(NULL);
    WNDCLASSA window = {0};

    window.lpfnWndProc = monitorConfigCallback;
    window.hInstance = instance;
    window.lpszClassName = windowClass;

    if (!RegisterClassA(&window)) {
        printf("Window registration failed.\n");
        return;
    }

    HWND hwnd = CreateWindowExA(
        0,
        windowClass,
        "Monitor Config Tracker",
        0,
        0, 0, 0, 0,
        NULL,
        NULL,
        instance,
        NULL
    );

    if (!hwnd) {
        printf("Window creation failed.\n");
        UnregisterClassA(windowClass, instance);
        return;
    }

    printf("Monitor Config Tracker running.\n");
    printf("Waiting for display configuration changes...\n");

    MSG message;

    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    DestroyWindow(hwnd);
    UnregisterClassA(windowClass, instance);
}