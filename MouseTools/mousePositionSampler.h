#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <stddef.h>

typedef struct {
  POINT position;
  ULONGLONG timestampMs;
} MousePositionSample;

typedef enum {
  MOUSE_POSITION_SAMPLER_ERROR = -1,
  MOUSE_POSITION_SAMPLER_NOT_DUE = 0,
  MOUSE_POSITION_SAMPLER_CAPTURED = 1
} MousePositionSamplerResult;

typedef struct {
  MousePositionSample *samples;
  size_t sampleCount;
  size_t sampleCapacity;
  DWORD intervalMs;
  ULONGLONG startTimeMs;
  ULONGLONG lastSampleTimeMs;
  BOOL hasSampled;
  BOOL isSampling;
} MousePositionSampler;

BOOL mousePositionSamplerInitialize(MousePositionSampler *sampler,
                                    DWORD intervalMs);
BOOL mousePositionSamplerStart(MousePositionSampler *sampler);
MousePositionSamplerResult
mousePositionSamplerUpdate(MousePositionSampler *sampler);
void mousePositionSamplerStop(MousePositionSampler *sampler);
void mousePositionSamplerClear(MousePositionSampler *sampler);
void mousePositionSamplerDestroy(MousePositionSampler *sampler);
const MousePositionSample *
mousePositionSamplerGetSamples(const MousePositionSampler *sampler,
                               size_t *sampleCount);