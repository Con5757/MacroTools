#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

static int parseInt(const char *text, int *result) {
    char *end;
    errno = 0;

    long value = strtol(text, &end, 10);
    if (errno || end == text || *end != '\0' || value < 0 || value > INT_MAX)
        return 0;

    *result = (int)value;
    return 1; 
}

static void printScreenInfo(int width, int height, int x, int y) {
    printf("\nMonitor Res: %d x %d\n", width, height);
    printf("Loc/Coord: %d x %d\n", x, y);
    printf("Monitor Pos: %.2f%% x %.2f%%\n", (double)x / width * 100, (double)y / height * 100);
    printf("\n");
}

static int readPixel(HDC screen, int x, int y, COLORREF *color) {
    COLORREF result = GetPixel(screen, x, y);
    if (result == CLR_INVALID)
        return 0;

    *color = result;
    return 1;
}

static void printColor(COLORREF color) {
    unsigned char red = GetRValue(color);
    unsigned char green = GetGValue(color);
    unsigned char blue = GetBValue(color);

    printf("hex: #%02X%02X%02X\n", red, green, blue);
    printf("rgb: %u, %u, %u", red, green, blue);
}

static void monitorPixel() {

}

int main() {


    return 0;
}

/*
// one pixel
colorLocator sample 260 123

// one pixel a second
colorLocator watch 260 123

// 100 miliseconds or other timing
colorLocator watch 260 123 100
*/