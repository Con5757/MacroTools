#pragma once
#include "visualToolsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VT_TEMPLATE_NAME_CAPACITY 64

typedef struct {
  char name[VT_TEMPLATE_NAME_CAPACITY];
  VTImage image;
} VTStoredTemplate;
typedef struct {
  VTStoredTemplate *items;
  size_t count;
  size_t capacity;
} VTTemplateLibrary;

VTStatus vtTemplateLibraryInitialize(VTTemplateLibrary *library);
void vtTemplateLibraryDestroy(VTTemplateLibrary *library);
VTStatus vtTemplateLibraryAdd(VTTemplateLibrary *library, const char *name,
                              const VTImage *image);
BOOL vtTemplateLibraryRemove(VTTemplateLibrary *library, const char *name);
const VTImage *vtTemplateLibraryGet(const VTTemplateLibrary *library,
                                    const char *name);
VTStatus vtTemplateLibrarySave(const VTTemplateLibrary *library,
                               const char *path);
VTStatus vtTemplateLibraryLoad(VTTemplateLibrary *library, const char *path);
#ifdef __cplusplus
}
#endif