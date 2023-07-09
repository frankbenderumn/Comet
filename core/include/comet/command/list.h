#ifndef COMET_COMMAND_LIST_H_
#define COMET_COMMAND_LIST_H_

#include "comet/index.h"
#include "comet/package.h"

void list(bool verbose_flag) {
    Package** packages = NULL;
    int row_length = 0;
    size_t packages_sz = index_load(&packages, &row_length);
    if (verbose_flag) {
        packages_list_verbose(packages, packages_sz);
    } else {
        packages_list(packages, packages_sz);
    }

    package_array_destruct(packages, packages_sz);
}

#endif