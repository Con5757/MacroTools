#define WIN32_LEAN_AND_MEAN
#include "screenCaptureProfiler.h"

void screenCaptureProfilerInitialize(ScreenCaptureProfiler *profiler) {
  if (profiler != NULL)
    ZeroMemory(profiler, sizeof(*profiler));
}
void screenCaptureProfilerReset(ScreenCaptureProfiler *profiler) {
  screenCaptureProfilerInitialize(profiler);
}
void screenCaptureProfilerRecord(ScreenCaptureProfiler *profiler,
                                 double elapsedMilliseconds) {
  if (profiler == NULL || elapsedMilliseconds < 0.0)
    return;
  if (profiler->sampleCount == 0 ||
      elapsedMilliseconds < profiler->minimumMilliseconds)
    profiler->minimumMilliseconds = elapsedMilliseconds;
  if (profiler->sampleCount == 0 ||
      elapsedMilliseconds > profiler->maximumMilliseconds)
    profiler->maximumMilliseconds = elapsedMilliseconds;
  profiler->totalMilliseconds += elapsedMilliseconds;
  profiler->sampleCount++;
}

BOOL screenCaptureProfilerRunSample(ScreenCaptureProfiler *profiler,
                                    ScreenCaptureOperation operation,
                                    void *context) {
  if (profiler == NULL || operation == NULL)
    return FALSE;
  LARGE_INTEGER frequency, start, finish;
  if (!QueryPerformanceFrequency(&frequency) || frequency.QuadPart == 0 ||
      !QueryPerformanceCounter(&start))
    return FALSE;
  CaptureImage image = {0};
  BOOL success = operation(context, &image);
  BOOL measured = QueryPerformanceCounter(&finish);
  captureImageFree(&image);
  if (!measured || !success)
    return FALSE;
  double elapsed = ((double)(finish.QuadPart - start.QuadPart) * 1000.0) /
                   (double)frequency.QuadPart;
  screenCaptureProfilerRecord(profiler, elapsed);
  return TRUE;
}

BOOL screenCaptureProfilerGetResult(const ScreenCaptureProfiler *profiler,
                                    ScreenCaptureProfileResult *result) {
  if (profiler == NULL || result == NULL || profiler->sampleCount == 0)
    return FALSE;
  result->sampleCount = profiler->sampleCount;
  result->totalMilliseconds = profiler->totalMilliseconds;
  result->averageMilliseconds =
      profiler->totalMilliseconds / (double)profiler->sampleCount;
  result->minimumMilliseconds = profiler->minimumMilliseconds;
  result->maximumMilliseconds = profiler->maximumMilliseconds;
  return TRUE;
}