#define WIN32_LEAN_AND_MEAN
#include "mouseTracker.h"

#include "mouseTracker/mouseCoordinateTracking.h"
#include "mouseTracker/mousePositionTracking.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INTERVAL_MS 100u
#define MIN_INTERVAL_MS 1u
#define MAX_INTERVAL_MS 60000u

BOOL mouseTrackerInitialize(MouseTracker *tracker, DWORD intervalMs) {
  if (tracker == NULL || intervalMs < MIN_INTERVAL_MS ||
      intervalMs > MAX_INTERVAL_MS) {
    return FALSE;
  }

  tracker->intervalMs = intervalMs;
  tracker->lastSampleTime = 0;
  tracker->hasSample = FALSE;

  return TRUE;
}

BOOL mouseTrackerUpdate(MouseTracker *tracker, MouseTrackerData *data) {
  if (tracker == NULL || data == NULL) {
    return FALSE;
  }

  ULONGLONG currentTime = GetTickCount64();

  if (tracker->hasSample &&
      currentTime - tracker->lastSampleTime < tracker->intervalMs) {
    return FALSE;
  }

  ZeroMemory(data, sizeof(*data));

  if (!mousePositionTracking(&data->desktopPosition)) {
    return FALSE;
  }

  data->monitorFound =
      mouseMonitorTracking(&data->desktopPosition, &data->monitor);

  if (data->monitorFound) {
    if (!mouseCoordinateTracking(&data->desktopPosition, &data->monitor.bounds,
                                 &data->monitorPosition)) {
      return FALSE;
    }
  }

  tracker->lastSampleTime = currentTime;
  tracker->hasSample = TRUE;

  return TRUE;
}

static BOOL readSamplingInterval(DWORD *intervalMs) {
  char input[64];

  printf("Sampling interval in milliseconds [100]: ");

  if (fgets(input, sizeof(input), stdin) == NULL) {
    return FALSE;
  }

  char *start = input;
  while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n') {
    start++;
  }

  if (*start == '\0') {
    *intervalMs = DEFAULT_INTERVAL_MS;
    return TRUE;
  }

  errno = 0;
  char *end = NULL;
  unsigned long value = strtoul(start, &end, 10);

  while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') {
    end++;
  }

  if (errno == ERANGE || end == start || *end != '\0' ||
      value < MIN_INTERVAL_MS || value > MAX_INTERVAL_MS) {
    printf("Invalid interval. Enter a value between %u and %u ms.\n",
           MIN_INTERVAL_MS, MAX_INTERVAL_MS);
    return FALSE;
  }

  *intervalMs = (DWORD)value;
  return TRUE;
}

void mouseTracker(void) {
  DWORD intervalMs;

  printf("\nMouse Tracker\n");
  printf("Press Ctrl+C to stop.\n\n");

  if (!readSamplingInterval(&intervalMs)) {
    return;
  }

  MouseTracker tracker;
  if (!mouseTrackerInitialize(&tracker, intervalMs)) {
    printf("Could not initialize the mouse tracker.\n");
    return;
  }

  printf("\nTracking every %lu ms...\n\n", (unsigned long)intervalMs);

  for (;;) {
    MouseTrackerData data;

    if (mouseTrackerUpdate(&tracker, &data)) {
      if (data.monitorFound) {
        printf("Desktop: (%ld, %ld) | Monitor: %s | "
               "Relative: (%ld, %ld) | Resolution: %ldx%ld\n",
               data.desktopPosition.x, data.desktopPosition.y,
               data.monitor.name, data.monitorPosition.x,
               data.monitorPosition.y, data.monitor.width, data.monitor.height);
      } else {
        printf("Desktop: (%ld, %ld) | Monitor: none | "
               "Relative: N/A\n",
               data.desktopPosition.x, data.desktopPosition.y);
      }
    }

    Sleep(1);
  }
}