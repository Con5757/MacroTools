#define WIN32_LEAN_AND_MEAN
#include "screenshotRecorder.h"
#include "screenCapture.h"

#include <stdio.h>
#include <string.h>

static BOOL ensureDirectory(const char *directoryUtf8) {
  WCHAR directoryWide[MAX_PATH];
  int converted = MultiByteToWideChar(CP_UTF8, 0, directoryUtf8, -1,
                                      directoryWide, MAX_PATH);
  if (converted <= 0 || converted >= MAX_PATH)
    return FALSE;
  if (CreateDirectoryW(directoryWide, NULL))
    return TRUE;
  return GetLastError() == ERROR_ALREADY_EXISTS;
}

BOOL screenshotRecorderStart(ScreenshotRecorder *recorder,
                             const char *directoryUtf8,
                             const char *filenamePrefix,
                             CaptureImageFormat format, DWORD intervalMs) {
  if (recorder == NULL || directoryUtf8 == NULL || directoryUtf8[0] == '\0' ||
      intervalMs == 0 || intervalMs > 3600000u ||
      captureImageFormatExtension(format) == NULL)
    return FALSE;

  size_t directoryLength = strlen(directoryUtf8);
  const char *prefix = (filenamePrefix != NULL && filenamePrefix[0] != '\0')
                           ? filenamePrefix
                           : "screenshot";
  if (directoryLength >= sizeof(recorder->directory) ||
      strlen(prefix) >= sizeof(recorder->prefix))
    return FALSE;
  if (!ensureDirectory(directoryUtf8))
    return FALSE;

  ZeroMemory(recorder, sizeof(*recorder));
  memcpy(recorder->directory, directoryUtf8, directoryLength + 1);
  memcpy(recorder->prefix, prefix, strlen(prefix) + 1);
  recorder->intervalMs = intervalMs;
  recorder->format = format;
  recorder->active = TRUE;
  return TRUE;
}

void screenshotRecorderStop(ScreenshotRecorder *recorder) {
  if (recorder != NULL)
    recorder->active = FALSE;
}

BOOL screenshotRecorderUpdate(ScreenshotRecorder *recorder, char *savedPath,
                              size_t savedPathCapacity) {
  if (recorder == NULL || !recorder->active)
    return FALSE;
  ULONGLONG now = GetTickCount64();
  if (recorder->hasCaptured &&
      now - recorder->lastCaptureMs < recorder->intervalMs)
    return FALSE;

  SYSTEMTIME time;
  GetLocalTime(&time);
  char path[SCREENSHOT_RECORDER_PATH_CAPACITY + 160];
  int length = snprintf(
      path, sizeof(path), "%s\\%s_%04u%02u%02u_%02u%02u%02u_%03u_%06lu.%s",
      recorder->directory, recorder->prefix, time.wYear, time.wMonth, time.wDay,
      time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
      recorder->sequence + 1, captureImageFormatExtension(recorder->format));
  if (length < 0 || (size_t)length >= sizeof(path))
    return FALSE;

  size_t pathLength = strlen(path);
  if (savedPath != NULL && savedPathCapacity > 0 &&
      pathLength + 1 > savedPathCapacity)
    return FALSE;

  CaptureImage image = {0};
  BOOL success = screenCaptureFullDesktop(&image) &&
                 captureImageSave(path, recorder->format, &image);
  captureImageFree(&image);
  if (!success)
    return FALSE;

  recorder->sequence++;
  recorder->lastCaptureMs = GetTickCount64();
  recorder->hasCaptured = TRUE;
  if (savedPath != NULL && savedPathCapacity > 0)
    memcpy(savedPath, path, pathLength + 1);
  return TRUE;
}