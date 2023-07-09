#ifndef COMET_COMMANDS_COMMAND_H_
#define COMET_COMMANDS_COMMAND_H_

#include "comet/setup.h"
#include "comet/manifest.h"

int command(PackageManifest* pm, const char* command_str);

#endif