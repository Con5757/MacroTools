#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  FILE *file;
  uint64_t eventsWritten;
} GTGameSessionLogger;

bool gameSessionLoggerOpen(GTGameSessionLogger *logger, const char *path);
bool gameSessionLoggerWrite(GTGameSessionLogger *logger, uint64_t timestampMs,
                            const char *category, const char *message);
void gameSessionLoggerClose(GTGameSessionLogger *logger);