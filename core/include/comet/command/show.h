#ifndef COMET_COMMAND_SHOW_H_
#define COMET_COMMAND_SHOW_H_

#include <string.h>

#include "comet/index.h"
#include "comet/package.h"

void show(const char* package_name) {

    Package** packages = 0;
    int row_length = 0;
    int packages_sz = index_load(&packages, &row_length);

    for (int i = 0; i < packages_sz; i++) {
        if (strcmp(packages[i]->manifest->name, package_name) == 0) {
            package_show(packages[i]);
            break;
        }
    }

    package_array_destruct(packages, packages_sz);

}

#endif