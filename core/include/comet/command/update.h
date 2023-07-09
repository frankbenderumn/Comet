#ifndef COMET_COMMAND_UPDATE_H_ 
#define COMET_COMMAND_UPDATE_H_ 

#include "comet/command/command.h"
#include "comet/index.h"
#include "comet/package.h"

void update(const char* package_name) {

    const char* MANIFEST_PATH = getenv("MANIFEST_PATH");
    if (MANIFEST_PATH == NULL) {
        printf("\033[1;31mError:\033[0m Manifest path not detected!\n");
        return;
    }

    PackageManifest** manifests = NULL;
    int manifests_sz = manifest_load(&manifests, MANIFEST_PATH);
    PackageManifest* target = manifest_get_by_name(manifests, manifests_sz, package_name);

    if (target == NULL) {
        printf("\033[1;31mError:\033[0m Package '%s' does not exist!\n", package_name);
        return;
    }

    Package** packages = NULL;
    int row_length = 0;
    int packages_sz = index_load(&packages, &row_length);

    bool found = false;
    for (int i = 0; i < packages_sz; i++) {
        if (strcmp(packages[i]->manifest->name, package_name) == 0) {
            found = true;
            break;
        }
    }

    if (!found) {
        printf("\033[1;31mError:\033[0m Package '%s' not installed!\n", package_name);
        package_array_destruct(packages, packages_sz);
        manifest_array_destruct(&manifests, manifests_sz);
        return;
    }

    if (command(target, "update") < 0) {
        printf("\033[1;31mError:\033[0m Failed to pipe update!\n");
        package_array_destruct(packages, packages_sz);
        manifest_array_destruct(&manifests, manifests_sz);
        return;
    }

    if (index_update(&packages, packages_sz, target) < 0) {
        printf("\033[1;31mError:\033[0m Failed to update index!\n");        
        package_array_destruct(packages, packages_sz);
        manifest_array_destruct(&manifests, manifests_sz);
        return;
    }

    package_array_destruct(packages, packages_sz);
    manifest_array_destruct(&manifests, manifests_sz);

}

#endif