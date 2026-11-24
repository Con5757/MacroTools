#pragma once

#include "gameToolsCommon.h"
#include <stdbool.h>
#include <stddef.h>

#define GT_TEMPLATE_MAGIC "GTMPL01"

typedef struct {
    char name[128];
    int width;
    int height;
    size_t stride;
    uint8_t *pixels;
} GTGameUITemplate;
typedef struct {
  char magic[8];
  char name[128];
  uint32_t width;
  uint32_t height;
  uint32_t stride;
} GTTemplateFileHeader;

bool gameUITemplateManagerSave(const char *path, const char *name,
                               const GTImage *image);
bool gameUITemplateManagerLoad(const char *path, GTGameUITemplate *templ);
void gameUITemplateManagerRelease(GTGameUITemplate *templ);
GTImage gameUITemplateManagerView(const GTGameUITemplate *templ);