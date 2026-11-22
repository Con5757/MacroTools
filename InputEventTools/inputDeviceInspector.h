#pragma once
#include "inputEventCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_DEVICE_NAME_CAPACITY 512

typedef enum InputDeviceType {
  INPUT_DEVICE_MOUSE = 0,
  INPUT_DEVICE_KEYBOARD = 1,
  INPUT_DEVICE_OTHER = 2
} InputDeviceType;
typedef struct InputDeviceInfo {
  HANDLE handle;
  InputDeviceType type;
  char name[INPUT_DEVICE_NAME_CAPACITY];
} InputDeviceInfo;

BOOL inputDeviceInspectorEnumerate(
    InputDeviceInfo *devices, size_t capacity,
    size_t *count); // returns false if capacity isnt applicable

#ifdef __cplusplus
}
#endif