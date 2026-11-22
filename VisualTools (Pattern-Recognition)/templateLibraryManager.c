#include "templateLibraryManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VT_LIBRARY_MAX_TEMPLATES 4096u
#define VT_LIBRARY_MAX_DIMENSION 8192u

static int findName(const VTTemplateLibrary *library, const char *name) {
  for (size_t i = 0; i < library->count; ++i) {
    if (strcmp(library->items[i].name, name) == 0) {
      return (int)i;
    }
  }
  return -1;
}
VTStatus vtTemplateLibraryInitialize(VTTemplateLibrary *library) {
  if (library == NULL)
    return VT_STATUS_INVALID_ARGUMENT;
  library->items = NULL;
  library->count = 0;
  library->capacity = 0;
  return VT_STATUS_OK;
}
void vtTemplateLibraryDestroy(VTTemplateLibrary *library) {
  if (library == NULL)
    return;
  for (size_t i = 0; i < library->count; ++i) {
    vtImageDestroy(&library->items[i].image);
  }
  free(library->items);
  library->items = NULL;
  library->count = 0;
  library->capacity = 0;
}
VTStatus vtTemplateLibraryAdd(VTTemplateLibrary *library, const char *name,
                              const VTImage *image) {
  if (library == NULL || name == NULL || name[0] == '\0' ||
      strlen(name) >= VT_TEMPLATE_NAME_CAPACITY || !vtImageIsValid(image) ||
      findName(library, name) >= 0 ||
      library->count >= VT_LIBRARY_MAX_TEMPLATES) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  if (library->count == library->capacity) {
    size_t newCapacity = library->capacity == 0 ? 8u : library->capacity * 2u;
    if (newCapacity > VT_LIBRARY_MAX_TEMPLATES) {
      newCapacity = VT_LIBRARY_MAX_TEMPLATES;
    }
    VTStoredTemplate *items = (VTStoredTemplate *)realloc(
        library->items, newCapacity * sizeof(*items));
    if (items == NULL)
      return VT_STATUS_OUT_OF_MEMORY;
    library->items = items;
    library->capacity = newCapacity;
  }

  VTStoredTemplate item;
  memset(&item, 0, sizeof(item));
  memcpy(item.name, name, strlen(name) + 1u);
  VTStatus status = vtImageClone(image, &item.image);
  if (status != VT_STATUS_OK)
    return status;
  library->items[library->count++] = item;
  return VT_STATUS_OK;
}
BOOL vtTemplateLibraryRemove(VTTemplateLibrary *library, const char *name) {
  if (library == NULL || name == NULL)
    return FALSE;
  int index = findName(library, name);
  if (index < 0)
    return FALSE;

  vtImageDestroy(&library->items[index].image);
  for (size_t i = (size_t)index + 1u; i < library->count; ++i) {
    library->items[i - 1u] = library->items[i];
  }
  --library->count;
  return TRUE;
}
const VTImage *vtTemplateLibraryGet(const VTTemplateLibrary *library,
                                    const char *name) {
  if (library == NULL || name == NULL)
    return NULL;
  int index = findName(library, name);
  return index < 0 ? NULL : &library->items[index].image;
}
VTStatus vtTemplateLibrarySave(const VTTemplateLibrary *library,
                               const char *path) {
  if (library == NULL || path == NULL || path[0] == '\0' ||
      library->count > VT_LIBRARY_MAX_TEMPLATES) {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  FILE *file = fopen(path, "wb");
  if (file == NULL)
    return VT_STATUS_IO_ERROR;

  const char magic[8] = {'V', 'T', 'M', 'P', 'L', '0', '1', '\0'};
  uint32_t count = (uint32_t)library->count;
  BOOL ok = fwrite(magic, 1u, sizeof(magic), file) == sizeof(magic) &&
            fwrite(&count, sizeof(count), 1u, file) == 1u;

  for (size_t i = 0; ok && i < library->count; ++i) {
    const VTStoredTemplate *item = &library->items[i];
    uint32_t width = (uint32_t)item->image.width;
    uint32_t height = (uint32_t)item->image.height;
    ok = vtImageIsValid(&item->image) &&
         fwrite(item->name, 1u, VT_TEMPLATE_NAME_CAPACITY, file) ==
             VT_TEMPLATE_NAME_CAPACITY &&
         fwrite(&width, sizeof(width), 1u, file) == 1u &&
         fwrite(&height, sizeof(height), 1u, file) == 1u;

    for (int y = 0; ok && y < item->image.height; ++y) {
      const uint8_t *row =
          item->image.pixels + (size_t)y * (size_t)item->image.stride;
      ok = fwrite(row, 3u, (size_t)item->image.width, file) ==
           (size_t)item->image.width;
    }
  }

  if (fclose(file) != 0)
    ok = FALSE;
  return ok ? VT_STATUS_OK : VT_STATUS_IO_ERROR;
}
VTStatus vtTemplateLibraryLoad(VTTemplateLibrary *library, const char *path) {
  if (library == NULL || path == NULL || path[0] == '\0') {
    return VT_STATUS_INVALID_ARGUMENT;
  }

  FILE *file = fopen(path, "rb");
  if (file == NULL)
    return VT_STATUS_IO_ERROR;

  char magic[8];
  uint32_t count = 0;
  BOOL ok = fread(magic, 1u, sizeof(magic), file) == sizeof(magic) &&
            memcmp(magic, "VTMPL01", 7u) == 0 &&
            fread(&count, sizeof(count), 1u, file) == 1u &&
            count <= VT_LIBRARY_MAX_TEMPLATES;

  VTTemplateLibrary temporary = {0};
  vtTemplateLibraryInitialize(&temporary);

  for (uint32_t i = 0; ok && i < count; ++i) {
    char name[VT_TEMPLATE_NAME_CAPACITY];
    uint32_t width = 0;
    uint32_t height = 0;
    ok = fread(name, 1u, sizeof(name), file) == sizeof(name) &&
         fread(&width, sizeof(width), 1u, file) == 1u &&
         fread(&height, sizeof(height), 1u, file) == 1u;
    name[sizeof(name) - 1u] = '\0';

    if (!ok || width == 0 || height == 0 || width > VT_LIBRARY_MAX_DIMENSION ||
        height > VT_LIBRARY_MAX_DIMENSION || name[0] == '\0') {
      ok = FALSE;
      break;
    }

    VTImage image = {0};
    VTStatus status = vtImageCreate(&image, (int)width, (int)height);
    if (status != VT_STATUS_OK) {
      ok = FALSE;
      break;
    }

    for (uint32_t y = 0; ok && y < height; ++y) {
      uint8_t *row = image.pixels + (size_t)y * (size_t)image.stride;
      ok = fread(row, 3u, width, file) == width;
    }

    if (ok) {
      ok = vtTemplateLibraryAdd(&temporary, name, &image) == VT_STATUS_OK;
    }
    vtImageDestroy(&image);
  }

  if (fclose(file) != 0)
    ok = FALSE;
  if (!ok) {
    vtTemplateLibraryDestroy(&temporary);
    return VT_STATUS_IO_ERROR;
  }

  vtTemplateLibraryDestroy(library);
  *library = temporary;
  return VT_STATUS_OK;
}