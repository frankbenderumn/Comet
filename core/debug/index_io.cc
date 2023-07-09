#include <stdlib.h>
#include <string.h>

#include "comet/index.h"
#include "comet/color.h"
#include "comet/package.h"
#include "comet/io.h"

int main(int argc, char* argv[]) {
    Package** packages = 0;
    int row_length = 0;
    int packages_sz = index_load(&packages, &row_length);
    YEL("Packages size: %i\n", packages_sz);
    packages_list(packages, packages_sz);

    package_array_destruct(packages, packages_sz);

    return 0;
}