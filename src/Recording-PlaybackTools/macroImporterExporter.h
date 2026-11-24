#pragma once
#include "macroToolsCommon.h"

// utf-8 interC-format -> import should change desination on success
bool macroImporterExporterExport(const char *path,
                                 const MacroSequence *sequence);
bool macroImporterExporterImport(const char *path, MacroSequence *destination);