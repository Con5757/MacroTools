#include "coordinateProfileManager.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CTStatus ctValidateCoordinateProfile(const CTCoordinateProfile *profile) {
  if (profile == NULL ||
      memchr(profile->name, '\0', sizeof(profile->name)) == NULL ||
      profile->name[0] == '\0' || !ctRectIsValid(profile->sourceBounds) ||
      !ctRectIsValid(profile->targetBounds) ||
      !ctPointIsFinite(profile->offset)) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  for (size_t i = 0; profile->name[i] != '\0'; ++i) {
    if (profile->name[i] == '\n' || profile->name[i] == '\r') {
      return CT_STATUS_INVALID_ARGUMENT;
    }
  }

  return CT_STATUS_OK;
}
static bool ctReadLine(FILE *file, char *buffer, size_t capacity) {
  if (fgets(buffer, (int)capacity, file) == NULL) {
    return false;
  }

  size_t length = strlen(buffer);
  const bool hasNewline = length > 0 && buffer[length - 1] == '\n';
  if (!hasNewline && !feof(file)) {
    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF) {
    }
    return false;
  }

  if (hasNewline) {
    buffer[--length] = '\0';
  }
  if (length > 0 && buffer[length - 1] == '\r') {
    buffer[length - 1] = '\0';
  }

  return true;
}
static bool ctParseNumbers(const char *line, double *values, size_t count) {
  const char *cursor = line;

  for (size_t i = 0; i < count; ++i) {
    while (isspace((unsigned char)*cursor)) {
      ++cursor;
    }

    errno = 0;
    char *end = NULL;
    const double value = strtod(cursor, &end);
    if (end == cursor || errno == ERANGE || !ctIsFinite(value)) {
      return false;
    }

    values[i] = value;
    cursor = end;
  }

  while (isspace((unsigned char)*cursor)) {
    ++cursor;
  }

  return *cursor == '\0';
}
CTStatus ctSaveCoordinateProfile(const char *path,
                                 const CTCoordinateProfile *profile) {
  if (path == NULL || path[0] == '\0' ||
      ctValidateCoordinateProfile(profile) != CT_STATUS_OK) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  FILE *file = fopen(path, "w");
  if (file == NULL) {
    return CT_STATUS_IO_ERROR;
  }

  int result = fprintf(file,
                       "COORDINATE_PROFILE 1\n"
                       "%s\n"
                       "%.17g %.17g %.17g %.17g\n"
                       "%.17g %.17g %.17g %.17g\n"
                       "%.17g %.17g\n",
                       profile->name, profile->sourceBounds.left,
                       profile->sourceBounds.top, profile->sourceBounds.right,
                       profile->sourceBounds.bottom, profile->targetBounds.left,
                       profile->targetBounds.top, profile->targetBounds.right,
                       profile->targetBounds.bottom, profile->offset.x,
                       profile->offset.y);

  const int closeResult = fclose(file);
  if (result < 0 || closeResult != 0) {
    return CT_STATUS_IO_ERROR;
  }

  return CT_STATUS_OK;
}
CTStatus ctLoadCoordinateProfile(const char *path,
                                 CTCoordinateProfile *profile) {
  if (path == NULL || path[0] == '\0' || profile == NULL) {
    return CT_STATUS_INVALID_ARGUMENT;
  }

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    return CT_STATUS_IO_ERROR;
  }

  CTCoordinateProfile result;
  memset(&result, 0, sizeof(result));
  char line[256];
  char nameLine[256];
  double values[8];
  CTStatus status = CT_STATUS_INVALID_FORMAT;

  if (!ctReadLine(file, line, sizeof(line)) ||
      strcmp(line, "COORDINATE_PROFILE 1") != 0 ||
      !ctReadLine(file, nameLine, sizeof(nameLine)) || nameLine[0] == '\0' ||
      strlen(nameLine) >= sizeof(result.name) ||
      !ctReadLine(file, line, sizeof(line)) ||
      !ctParseNumbers(line, values, 4)) {
    goto cleanup;
  }

  memcpy(result.name, nameLine, strlen(nameLine) + 1);
  result.sourceBounds = (CTRect){values[0], values[1], values[2], values[3]};

  if (!ctReadLine(file, line, sizeof(line)) ||
      !ctParseNumbers(line, values, 4)) {
    goto cleanup;
  }
  result.targetBounds = (CTRect){values[0], values[1], values[2], values[3]};

  if (!ctReadLine(file, line, sizeof(line)) ||
      !ctParseNumbers(line, values, 2)) {
    goto cleanup;
  }
  result.offset = (CTPoint){values[0], values[1]};

  int ch;
  while ((ch = fgetc(file)) != EOF) {
    if (!isspace((unsigned char)ch)) {
      goto cleanup;
    }
  }
  if (ferror(file)) {
    status = CT_STATUS_IO_ERROR;
    goto cleanup;
  }

  status = ctValidateCoordinateProfile(&result);
  if (status == CT_STATUS_OK) {
    *profile = result;
  } else {
    status = CT_STATUS_INVALID_FORMAT;
  }

cleanup:
  if (fclose(file) != 0 && status == CT_STATUS_OK) {
    status = CT_STATUS_IO_ERROR;
  }
  return status;
}