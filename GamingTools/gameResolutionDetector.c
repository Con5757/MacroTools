#include "gameResolutionDetector.h"

#ifdef _WIN32
bool gameResolutionDetectorRead(GTWindowHandle window,
                                GTGameResolution *resolution) {
  if (window == NULL || resolution == NULL || !IsWindow(window))
    return false;
  RECT client;
  POINT origin = {0, 0};
  if (!GetClientRect(window, &client) || !ClientToScreen(window, &origin))
    return false;
  int width = client.right - client.left;
  int height = client.bottom - client.top;
  if (width <= 0 || height <= 0)
    return false;
  resolution->width = width;
  resolution->height = height;
  resolution->aspectRatio = (double)width / (double)height;
  resolution->desktopX = origin.x;
  resolution->desktopY = origin.y;
  return true;
}
#else
bool gameResolutionDetectorRead(GTWindowHandle window,
                                GTGameResolution *resolution) {
  (void)window;
  (void)resolution;
  return false;
}
#endif