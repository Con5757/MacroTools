#define WIN32_LEAN_AND_MEAN
#include "mousePositionSampler.h"

#include <stdint.h>
#include <stdlib.h>

#define MOUSE_POSITION_SAMPLER_INITIAL_CAPACITY 256u
#define MOUSE_POSITION_SAMPLER_MIN_INTERVAL_MS 1u
#define MOUSE_POSITION_SAMPLER_MAX_INTERVAL_MS 60000u

static BOOL mousePositionSamplerReserve(MousePositionSampler *sampler,
                                        size_t required) {
  if (required <= sampler->sampleCapacity) {
    return TRUE;
  }

  size_t capacity = sampler->sampleCapacity
                        ? sampler->sampleCapacity
                        : MOUSE_POSITION_SAMPLER_INITIAL_CAPACITY;
  while (capacity < required) {
    if (capacity > SIZE_MAX / 2) {
      capacity = required;
      break;
    }
    capacity *= 2;
  }

  if (capacity > SIZE_MAX / sizeof(*sampler->samples)) {
    return FALSE;
  }

  MousePositionSample *samples = (MousePositionSample *)realloc(
      sampler->samples, capacity * sizeof(*samples));
  if (samples == NULL) {
    return FALSE;
  }

  sampler->samples = samples;
  sampler->sampleCapacity = capacity;
  return TRUE;
}

BOOL mousePositionSamplerInitialize(MousePositionSampler *sampler,
                                    DWORD intervalMs) {
  if (sampler == NULL || intervalMs < MOUSE_POSITION_SAMPLER_MIN_INTERVAL_MS ||
      intervalMs > MOUSE_POSITION_SAMPLER_MAX_INTERVAL_MS) {
    return FALSE;
  }

  ZeroMemory(sampler, sizeof(*sampler));
  sampler->intervalMs = intervalMs;
  return TRUE;
}

BOOL mousePositionSamplerStart(MousePositionSampler *sampler) {
  if (sampler == NULL || sampler->intervalMs == 0) {
    return FALSE;
  }

  sampler->sampleCount = 0;
  sampler->startTimeMs = GetTickCount64();
  sampler->lastSampleTimeMs = 0;
  sampler->hasSampled = FALSE;
  sampler->isSampling = TRUE;
  return TRUE;
}

MousePositionSamplerResult
mousePositionSamplerUpdate(MousePositionSampler *sampler) {
  if (sampler == NULL) {
    return MOUSE_POSITION_SAMPLER_ERROR;
  }
  if (!sampler->isSampling) {
    return MOUSE_POSITION_SAMPLER_NOT_DUE;
  }

  ULONGLONG now = GetTickCount64();
  if (sampler->hasSampled &&
      now - sampler->lastSampleTimeMs < sampler->intervalMs) {
    return MOUSE_POSITION_SAMPLER_NOT_DUE;
  }

  POINT position;
  if (!GetCursorPos(&position) || sampler->sampleCount == SIZE_MAX ||
      !mousePositionSamplerReserve(sampler, sampler->sampleCount + 1)) {
    return MOUSE_POSITION_SAMPLER_ERROR;
  }

  MousePositionSample *sample = &sampler->samples[sampler->sampleCount++];
  sample->position = position;
  sample->timestampMs = now - sampler->startTimeMs;

  sampler->lastSampleTimeMs = now;
  sampler->hasSampled = TRUE;
  return MOUSE_POSITION_SAMPLER_CAPTURED;
}

void mousePositionSamplerStop(MousePositionSampler *sampler) {
  if (sampler != NULL) {
    sampler->isSampling = FALSE;
  }
}

void mousePositionSamplerClear(MousePositionSampler *sampler) {
  if (sampler != NULL) {
    sampler->sampleCount = 0;
    sampler->lastSampleTimeMs = 0;
    sampler->hasSampled = FALSE;
  }
}

void mousePositionSamplerDestroy(MousePositionSampler *sampler) {
  if (sampler == NULL) {
    return;
  }
  free(sampler->samples);
  ZeroMemory(sampler, sizeof(*sampler));
}

const MousePositionSample *
mousePositionSamplerGetSamples(const MousePositionSampler *sampler,
                               size_t *sampleCount) {
  if (sampleCount != NULL) {
    *sampleCount = sampler != NULL ? sampler->sampleCount : 0;
  }
  return sampler != NULL ? sampler->samples : NULL;
}