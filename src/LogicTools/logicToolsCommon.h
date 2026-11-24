#pragma once

#ifndef LOGIC_TOOLS_COMMON_H
#define LOGIC_TOOLS_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LA_NAME_CAPACITY 48
#define LA_MAX_ITEMS 64
#define LA_INVALID_ID UINT32_MAX

typedef enum { LA_VALUE_NUMBER = 0, LA_VALUE_BOOLEAN = 1 } LAValueType;
typedef struct {
  LAValueType type;
  union {
    double number;
    bool boolean;
  } data;
} LAValue;
typedef bool (*LAConditionCallback)(void *context);
typedef void (*LAActionCallback)(void *context, uint32_t actionId,
                                 double value);
typedef bool (*LAValueResolver)(void *context, const char *name, double *value);

bool laCopyName(char destination[LA_NAME_CAPACITY], const char *source);
uint64_t laAddMilliseconds(uint64_t startMs, uint64_t durationMs);

#endif