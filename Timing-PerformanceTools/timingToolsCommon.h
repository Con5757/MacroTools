#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <stdint.h>

BOOL ttQueryPerformanceCounter(LARGE_INTEGER *counter);
BOOL ttQueryPerformanceFrequency(LARGE_INTEGER *frequency);
double ttCounterTicksToMilliseconds(LONGLONG ticks);
double ttCounterDifferenceMilliseconds(LARGE_INTEGER start, LARGE_INTEGER end);
uint64_t ttFileTimeToUInt64(FILETIME value);
double ttFileTimeDifferenceMilliseconds(FILETIME start, FILETIME end);
DWORD ttGetProcessorCount(void);