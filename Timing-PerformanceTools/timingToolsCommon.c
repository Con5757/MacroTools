#include "timingToolsCommon.h"
#include <stddef.h>

BOOL ttQueryPerformanceCounter(LARGE_INTEGER *counter) {
  return counter != NULL && QueryPerformanceCounter(counter);
}
BOOL ttQueryPerformanceFrequency(LARGE_INTEGER *frequency) {
  return frequency != NULL && QueryPerformanceFrequency(frequency) &&
         frequency->QuadPart > 0;
}
double ttCounterTicksToMilliseconds(LONGLONG ticks) {
  LARGE_INTEGER frequency;
  if (!ttQueryPerformanceFrequency(&frequency))
    return 0.0;
  return ((double)ticks * 1000.0) / (double)frequency.QuadPart;
}
double ttCounterDifferenceMilliseconds(LARGE_INTEGER start, LARGE_INTEGER end) {
  return ttCounterTicksToMilliseconds(end.QuadPart - start.QuadPart);
}
uint64_t ttFileTimeToUInt64(FILETIME value) {
  ULARGE_INTEGER converted;
  converted.LowPart = value.dwLowDateTime;
  converted.HighPart = value.dwHighDateTime;
  return converted.QuadPart;
}
double ttFileTimeDifferenceMilliseconds(FILETIME start, FILETIME end) {
  uint64_t a = ttFileTimeToUInt64(start);
  uint64_t b = ttFileTimeToUInt64(end);
  return b >= a ? (double)(b - a) / 10000.0 : 0.0;
}
DWORD ttGetProcessorCount(void) {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors ? info.dwNumberOfProcessors : 1;
}