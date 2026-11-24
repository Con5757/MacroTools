#include "gameSessionLogger.h"

static bool writeCsvField(FILE *file, const char *text) {
  if (text == NULL)
    text = "";
  if (fputc('"', file) == EOF)
    return false;
  for (const char *p = text; *p; ++p) {
    if (*p == '"' && fputc('"', file) == EOF)
      return false;
    if (fputc((unsigned char)*p, file) == EOF)
      return false;
  }
  return fputc('"', file) != EOF;
}
bool gameSessionLoggerOpen(GTGameSessionLogger *logger, const char *path) {
  if (logger == NULL || path == NULL)
    return false;
  FILE *file = fopen(path, "a");
  if (file == NULL)
    return false;
  logger->file = file;
  logger->eventsWritten = 0;
  return true;
}
bool gameSessionLoggerWrite(GTGameSessionLogger *logger, uint64_t timestampMs,
                            const char *category, const char *message) {
  if (logger == NULL || logger->file == NULL)
    return false;
  if (fprintf(logger->file, "%llu,", (unsigned long long)timestampMs) < 0 ||
      !writeCsvField(logger->file, category) ||
      fputc(',', logger->file) == EOF ||
      !writeCsvField(logger->file, message) ||
      fputc('\n', logger->file) == EOF || fflush(logger->file) != 0)
    return false;
  logger->eventsWritten++;
  return true;
}
void gameSessionLoggerClose(GTGameSessionLogger *logger) {
  if (logger == NULL)
    return;
  if (logger->file != NULL)
    fclose(logger->file);
  logger->file = NULL;
}