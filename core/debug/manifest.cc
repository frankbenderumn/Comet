#include "comet/manifest.h"

int main(int argc, char* argv[]) {
    BYEL("Debugging manifest...\n");

    PackageManifest** manifest = 0;
    int manifest_size = manifest_load(&manifest, "./manifest/manifest.json");

    YEL("Manifest parsed! size if %i\n", manifest_size);

    if (manifest == NULL) {
        BRED("Manifest is NULL\n");
        exit(1);
    }

    const char* package = "celerity";
    PackageManifest* p = manifest_get_by_name(manifest, manifest_size, package);

    if (p != NULL) {
        GRE("Package match found!\n");
        manifest_dump(p);
    } else {
        BRED("Package not found!\n");
    }

    for (int i = 0; i < p->deps_sz; i++) {
        YEL("Dep: %s\n", p->deps[i]);
    }

    return 0;
}