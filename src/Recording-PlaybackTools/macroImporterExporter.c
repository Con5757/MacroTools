#include "macroImporterExporter.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MACRO_IMPORT_MAX_STEPS 1000000u
static const char hexDigits[] = "0123456789ABCDEF";

static bool writeHex(FILE *f, const char *s) {
  if (!s || !*s)
    return fputc('-', f) != EOF;
  for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
    if (fputc(hexDigits[*p >> 4], f) == EOF ||
        fputc(hexDigits[*p & 15], f) == EOF)
      return false;
  }
  return true;
}
static int hexValue(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}
static bool readHex(const char *src, char *dst, size_t cap) {
  if (!src || !dst || cap == 0)
    return false;
  if (strcmp(src, "-") == 0) {
    dst[0] = '\0';
    return true;
  }
  size_t n = strlen(src);
  if ((n & 1u) || n / 2 >= cap)
    return false;
  for (size_t i = 0; i < n; i += 2) {
    int a = hexValue(src[i]), b = hexValue(src[i + 1]);
    if (a < 0 || b < 0)
      return false;
    dst[i / 2] = (char)((a << 4) | b);
    if (dst[i / 2] == '\0')
      return false;
  }
  dst[n / 2] = '\0';
  return true;
}
bool macroImporterExporterExport(const char *path, const MacroSequence *s) {
  if (!path || !s)
    return false;
  FILE *f = fopen(path, "wb");
  if (!f)
    return false;
  bool ok = fprintf(f, "MACROTOOLS 1\nNAMEHEX ") >= 0 && writeHex(f, s->name) &&
            fprintf(f, "\nCOUNT %zu\n", s->count) >= 0;
  for (size_t i = 0; ok && i < s->count; i++) {
    const MacroStep *p = &s->steps[i];
    ok = fprintf(f,
                 "STEP %" PRIu64 " %" PRIu32 " %" PRIu32 " %" PRIu64 " %" PRId32
                 " %" PRId32 " %" PRId32 " %" PRId32 " %.17g %" PRIu32
                 " %" PRIu32 " ",
                 p->id, p->type, p->flags, p->delay_ms, p->x, p->y, p->code,
                 p->value, p->number, p->target_step, p->alternate_step) >= 0 &&
         writeHex(f, p->text) && fputc('\n', f) != EOF;
  }
  if (fclose(f) != 0)
    ok = false;
  return ok;
}
static bool parseCount(const char *line, size_t *out) {
  unsigned long long v;
  char extra;
  if (sscanf(line, "COUNT %llu %c", &v, &extra) != 1 ||
      v > MACRO_IMPORT_MAX_STEPS || v > SIZE_MAX)
    return false;
  *out = (size_t)v;
  return true;
}
bool macroImporterExporterImport(const char *path, MacroSequence *destination) {
  if (!path || !destination)
    return false;
  FILE *f = fopen(path, "rb");
  if (!f)
    return false;
  MacroSequence tmp;
  macroSequenceInitialize(&tmp, NULL);
  char line[512], namehex[MACRO_NAME_CAPACITY * 2 + 1];
  bool ok = fgets(line, sizeof(line), f) != NULL &&
            strcmp(line, "MACROTOOLS 1\n") == 0;
  if (ok)
    ok = fgets(line, sizeof(line), f) != NULL &&
         sscanf(line, "NAMEHEX %128s", namehex) == 1 &&
         readHex(namehex, tmp.name, sizeof(tmp.name));
  size_t count = 0;
  if (ok)
    ok = fgets(line, sizeof(line), f) != NULL && parseCount(line, &count);
  if (ok)
    ok = macroSequenceReserve(&tmp, count);
  for (size_t i = 0; ok && i < count; i++) {
    if (!fgets(line, sizeof(line), f)) {
      ok = false;
      break;
    }
    unsigned long long id, delay;
    unsigned int type, flags, target, alternate;
    int x, y, code, value, n = 0;
    double number;
    char texthex[MACRO_TEXT_CAPACITY * 2 + 1];
    int fields =
        sscanf(line, "STEP %llu %u %u %llu %d %d %d %d %lf %u %u %192s %n", &id,
               &type, &flags, &delay, &x, &y, &code, &value, &number, &target,
               &alternate, texthex, &n);
    if (fields != 12 || line[n] != '\0' || type < MACRO_STEP_KEY_DOWN ||
        type > MACRO_STEP_CHECKPOINT) {
      ok = false;
      break;
    }
    MacroStep p;
    macroStepInitialize(&p, (MacroStepType)type);
    p.id = (uint64_t)id;
    p.flags = flags;
    p.delay_ms = (uint64_t)delay;
    p.x = x;
    p.y = y;
    p.code = code;
    p.value = value;
    p.number = number;
    p.target_step = target;
    p.alternate_step = alternate;
    if (!readHex(texthex, p.text, sizeof(p.text)) ||
        !macroSequenceAppend(&tmp, &p)) {
      ok = false;
      break;
    }
  }
  if (ok) {
    int ch;
    while ((ch = fgetc(f)) != EOF)
      if (ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t') {
        ok = false;
        break;
      }
  }
  if (fclose(f) != 0) {
    ok = false;
  }
  if (ok) {
    macroSequenceDestroy(destination);
    *destination = tmp;
  } else {
    macroSequenceDestroy(&tmp);
  }
  return ok;
}