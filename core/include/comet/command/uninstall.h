#ifndef COMET_COMMAND_UNINSTALL_H_
#define COMET_COMMAND_UNINSTALL_H_

#include "comet/command/command.h"
#include "comet/index.h"
#include "comet/package.h"

void uninstall(const char* package_name) {

    YEL("Uninstalling...\n");

    Package** packages = (Package**)calloc(1, sizeof(Package*));
    int row_length = 0;

    BLU("Hoorah!\n");

    int packages_sz = index_load(&packages, &row_length);

    BLU("Hoorah!\n");


    PackageManifest** manifests = 0;
    const char comet_manifest[] = "./manifest/manifest.json";

    int manifest_sz = manifest_load(&manifests, comet_manifest);

    PackageManifest* pm = manifest_get_by_name(manifests, manifest_sz, package_name);

    if (pm == NULL) {
        printf("\033[1;31mError:\033[0m Package '%s' does not exist!\n", package_name);
        manifest_array_destruct(&manifests, manifest_sz);
        package_array_destruct(packages, packages_sz);
        return;
    }

    if (command(pm, "uninstall") < 0) {
        manifest_array_destruct(&manifests, manifest_sz);
        package_array_destruct(packages, packages_sz);
        return;
    }

    BLU("Hoorah!\n");

    char* trace = NULL;
    if (index_remove(packages, packages_sz, (char*)package_name) < 0) {
        manifest_array_destruct(&manifests, manifest_sz);
        package_array_destruct(packages, packages_sz);
        return;
    }

    manifest_array_destruct(&manifests, manifest_sz);
    package_array_destruct(packages, packages_sz);

}

#endif