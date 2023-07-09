#ifndef COMET_COMMAND_GET_H_
#define COMET_COMMAND_GET_H_

#include "comet/command/command.h"

int get(PackageManifest* pm) {
    return command(pm, "get");
}

#endif