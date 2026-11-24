#include "gameWindowDetector.h"
#ifdef _WIN32
#include <ctype.h>
#include <string.h>

static bool containsIgnoreCase(const char *text, const char *needle) {
  if (needle == NULL || *needle == '\0')
    return true;
  if (text == NULL)
    return false;
  for (; *text; ++text) {
    const char *a = text;
    const char *b = needle;
    while (*a && *b &&
           tolower((unsigned char)*a) == tolower((unsigned char)*b)) {
      ++a;
      ++b;
    }
    if (*b == '\0')
      return true;
  }
  return false;
}
static BOOL CALLBACK enumerateWindow(HWND hwnd, LPARAM parameter) {
  GTWindowSearch *search = (GTWindowSearch *)parameter;
  if (!IsWindowVisible(hwnd))
    return TRUE;

  char title[256] = {0};
  int length = GetWindowTextA(hwnd, title, (int)sizeof(title));
  if (length <= 0 || !containsIgnoreCase(title, search->filter))
    return TRUE;

  if (search->count < search->capacity && search->windows != NULL) {
    RECT rect;
    RECT client;
    POINT clientOrigin = {0, 0};
    GTGameWindowInfo *entry = &search->windows[search->count];
    if (GetWindowRect(hwnd, &rect) && GetClientRect(hwnd, &client) &&
        ClientToScreen(hwnd, &clientOrigin)) {
      entry->handle = hwnd;
      strncpy(entry->title, title, sizeof(entry->title) - 1u);
      entry->x = clientOrigin.x;
      entry->y = clientOrigin.y;
      entry->width = client.right - client.left;
      entry->height = client.bottom - client.top;
      entry->visible = true;
    } else {
      return TRUE;
    }
  }
  search->count++;
  return TRUE;
}
bool gameWindowDetectorFind(const char *titleSubstring,
                            GTGameWindowInfo *windows, size_t capacity,
                            size_t *foundCount) {
  if (foundCount == NULL || (capacity > 0 && windows == NULL))
    return false;
  GTWindowSearch search = {titleSubstring, windows, capacity, 0};
  if (!EnumWindows(enumerateWindow, (LPARAM)&search))
    return false;
  *foundCount = search.count;
  return true;
}
#else
bool gameWindowDetectorFind(const char *titleSubstring,
                            GTGameWindowInfo *windows, size_t capacity,
                            size_t *foundCount) {
  (void)titleSubstring;
  (void)windows;
  (void)capacity;
  if (foundCount != NULL)
    *foundCount = 0;
  return false;
}
#endif