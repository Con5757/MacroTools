#include "gameUITemplateManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool gameUITemplateManagerSave(const char *path, const char *name,
                               const GTImage *image) {
  if (path == NULL || name == NULL || !gtImageIsValid(image) ||
      (uint64_t)image->width > UINT32_MAX ||
      (uint64_t)image->height > UINT32_MAX ||
      (size_t)image->width * 3u > UINT32_MAX)
    return false;
  FILE *file = fopen(path, "wb");
  if (file == NULL)
    return false;
  GTTemplateFileHeader header = {{0},
                                 {0},
                                 (uint32_t)image->width,
                                 (uint32_t)image->height,
                                 (uint32_t)((size_t)image->width * 3u)};
  memcpy(header.magic, GT_TEMPLATE_MAGIC, sizeof(GT_TEMPLATE_MAGIC));
  strncpy(header.name, name, sizeof(header.name) - 1u);
  bool ok = fwrite(&header, sizeof(header), 1, file) == 1;
  for (int y = 0; ok && y < image->height; ++y)
    ok = fwrite(image->pixels + (size_t)y * image->stride, header.stride, 1,
                file) == 1;
  if (fclose(file) != 0)
    ok = false;
  return ok;
}
bool gameUITemplateManagerLoad(const char *path, GTGameUITemplate *templ) {
  if (path == NULL || templ == NULL)
    return false;
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    return false;
  GTTemplateFileHeader header;
  if (fread(&header, sizeof(header), 1, file) != 1 ||
      memcmp(header.magic, GT_TEMPLATE_MAGIC, sizeof(GT_TEMPLATE_MAGIC)) != 0 ||
      header.width == 0 || header.height == 0 || header.width > INT32_MAX ||
      header.height > INT32_MAX || header.width > UINT32_MAX / 3u ||
      header.stride != header.width * 3u ||
      (size_t)header.height > SIZE_MAX / header.stride) {
    fclose(file);
    return false;
  }
  size_t bytes = (size_t)header.stride * header.height;
  uint8_t *pixels = (uint8_t *)malloc(bytes);
  if (pixels == NULL) {
    fclose(file);
    return false;
  }
  bool ok = fread(pixels, 1, bytes, file) == bytes;
  int trailing = fgetc(file);
  if (trailing != EOF)
    ok = false;
  fclose(file);
  if (!ok) {
    free(pixels);
    return false;
  }
  memset(templ, 0, sizeof(*templ));
  memcpy(templ->name, header.name, sizeof(templ->name));
  templ->name[sizeof(templ->name) - 1u] = '\0';
  templ->width = (int)header.width;
  templ->height = (int)header.height;
  templ->stride = header.stride;
  templ->pixels = pixels;
  return true;
}
void gameUITemplateManagerRelease(GTGameUITemplate *templ) {
  if (templ == NULL)
    return;
  free(templ->pixels);
  memset(templ, 0, sizeof(*templ));
}
GTImage gameUITemplateManagerView(const GTGameUITemplate *templ) {
  if (templ == NULL)
    return (GTImage){0, 0, 0, NULL};
  return (GTImage){templ->width, templ->height, templ->stride, templ->pixels};
}