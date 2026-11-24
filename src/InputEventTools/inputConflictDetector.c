#include "inputConflictDetector.h"

static size_t report(InputConflict *out, size_t cap, size_t n, size_t idx,
                     InputConflictKind kind, UINT code) {
  if (n < cap && out != NULL) {
    out[n].eventIndex = idx;
    out[n].kind = kind;
    out[n].code = code;
  }
  return n + 1;
}
size_t inputConflictDetectorAnalyze(const InputEvent *events, size_t count,
                                    InputConflict *output,
                                    size_t outputCapacity) {
  if (count != 0 && events == NULL)
    return 0;
  BYTE keys[256] = {0}, buttons[6] = {0};
  size_t conflicts = 0;
  for (size_t i = 0; i < count; ++i) {
    const InputEvent *e = &events[i];
    if (e->source == INPUT_EVENT_SOURCE_UNKNOWN)
      conflicts = report(output, outputCapacity, conflicts, i,
                         INPUT_CONFLICT_UNKNOWN_SOURCE, 0);
    if ((e->type == INPUT_EVENT_KEY_DOWN || e->type == INPUT_EVENT_KEY_UP) &&
        e->deviceHandle == NULL)
      conflicts = report(output, outputCapacity, conflicts, i,
                         INPUT_CONFLICT_MISSING_DEVICE, e->virtualKey);
    if (e->type == INPUT_EVENT_KEY_DOWN && e->virtualKey < 256) {
      if (keys[e->virtualKey])
        conflicts = report(output, outputCapacity, conflicts, i,
                           INPUT_CONFLICT_DUPLICATE_KEY_DOWN, e->virtualKey);
      keys[e->virtualKey] = 1;
    } else if (e->type == INPUT_EVENT_KEY_UP && e->virtualKey < 256) {
      if (!keys[e->virtualKey])
        conflicts = report(output, outputCapacity, conflicts, i,
                           INPUT_CONFLICT_KEY_UP_WITHOUT_DOWN, e->virtualKey);
      keys[e->virtualKey] = 0;
    } else if (e->type == INPUT_EVENT_MOUSE_BUTTON_DOWN &&
               e->mouseButton >= 1 && e->mouseButton <= 5) {
      if (buttons[e->mouseButton])
        conflicts =
            report(output, outputCapacity, conflicts, i,
                   INPUT_CONFLICT_DUPLICATE_BUTTON_DOWN, e->mouseButton);
      buttons[e->mouseButton] = 1;
    } else if (e->type == INPUT_EVENT_MOUSE_BUTTON_UP && e->mouseButton >= 1 &&
               e->mouseButton <= 5) {
      if (!buttons[e->mouseButton])
        conflicts =
            report(output, outputCapacity, conflicts, i,
                   INPUT_CONFLICT_BUTTON_UP_WITHOUT_DOWN, e->mouseButton);
      buttons[e->mouseButton] = 0;
    }
  }
  return conflicts;
}