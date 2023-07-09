#include "comet/command/install.h"
#include "comet/manifest.h"
#include "comet/command/get.h"

bool installed(Package** packages, int packages_sz, const char* name) {
    for (int i = 0; i < packages_sz; i++) {
        if (packages[i]) {
            if (strcmp(packages[i]->manifest->name, name) == 0) {
                return true;
            }
        }
    }
    return false;
}


void install(const char* package_name) {
    Package** packages = (Package**)calloc(1, sizeof(Package*));
    PackageManifest* p = 0;
    PackageManifest* pm = 0;
    PackageManifest** manifest = 0;
    int packages_sz = 0;
    int num_manifests = 0;
    int row_length = 0;

    packages_sz = index_load(&packages, &row_length);

    YEL("=======================================================\n" \
        "                     INITIAL INDEX                   \n" \
        "=======================================================\n");    
    index_dump(packages, packages_sz);

    num_manifests = manifest_load(&manifest, "./manifest/manifest.json");

    BBLU("========================\n");
    printf("     primary parse      \n");
    BBLU("========================\n");

    pm = manifest_get_by_name(manifest, num_manifests, package_name);

    if (pm != NULL) {

    if (!installed(packages, packages_sz, pm->name)) {
        if (pm->deps_sz > 0) {
            BWHI("Dependencies detected for package '%s' (%i)\n", package_name, pm->deps_sz);
            for (int i = 0; i < pm->deps_sz; i++) {
                BMAG("------------------------\n");
                printf("          %i             \n", packages_sz);
                BMAG("------------------------\n");
                p = manifest_get_by_name(manifest, num_manifests, pm->deps[i]);
                if (p == NULL) {
                    return;
                    BRED("Package dep '%s' does not exist!\n", pm->deps[i]);
                    goto cleanup;
               } else {
                    if (!installed(packages, packages_sz, p->name)) {
                        int status = get(p);
                        if (status == 2) {
                            index_verify(&packages, packages_sz, p->name);
                            goto cleanup;
                        } else if (status < 0) {
                            goto cleanup;
                        }
                        status = install_pipe(p);
                        if (status == 2) {
                            index_verify(&packages, packages_sz, p->name);
                            goto cleanup;
                        } else if (status < 0) {
                            goto cleanup;
                        }
                        packages_sz = index_add(&packages, packages_sz, p);
                    
                    } else {
                        printf("\033[1;36mInfo: Dep package '%s' already installed!\033[0m\n", p->name);
                    }
                }
            }

            int status = get(p);
            if (status == 2) {
                index_verify(&packages, packages_sz, p->name);
                goto cleanup;
            } else if (status < 0) {
                goto cleanup;
            }
            status = install_pipe(p);
            if (status == 2) {
                index_verify(&packages, packages_sz, p->name);
                goto cleanup;
            } else if (status < 0) {
                goto cleanup;
            }
            packages_sz = index_add(&packages, packages_sz, p);

        } else {

            int status = get(pm);
            if (status == 2) {
                index_verify(&packages, packages_sz, pm->name);
                goto cleanup;
            } else if (status < 0) {
                goto cleanup;
            }
            status = install_pipe(pm);
            if (status == 2) {
                index_verify(&packages, packages_sz, pm->name);
                goto cleanup;
            } else if (status < 0) {
                goto cleanup;
            }
            packages_sz = index_add(&packages, packages_sz, pm);

        }
    } else {
        printf("\033[1;36mInfo: Package '%s' already installed!\033[0m\n", pm->name);
        index_verify(&packages, packages_sz, pm->name);
    }

    }

    BWHI("===============================\n");
    printf("         Dumping Index        \n");
    BWHI("===============================\n");
    printf("%i\n", packages_sz);
    index_dump(packages, packages_sz);

cleanup:
    if (manifest) manifest_array_destruct(&manifest, num_manifests);
    // if (manifest) free(manifest);
    package_array_destruct(packages, packages_sz);
    GRE("Gracefully terminated!\n");
}

int install_pipe(PackageManifest* pm) {
    BYEL("Installing %s...\n", pm->name);

    char* comet_dir = getenv("COMET_DIR");
    size_t script_path_sz = strlen(comet_dir) + strlen("/.internal/manifest/script/") + strlen(pm->source);
    char script_path[script_path_sz + 1];
    snprintf(script_path, script_path_sz + 1, "%s%s%s", comet_dir, "/.internal/manifest/script/", pm->source);
    script_path[script_path_sz] = 0;
    int status;

    char** passables = (char**)malloc(sizeof(char*) * 9);
    passables[0] = strdup("sh");
    passables[1] = strdup(script_path);
    passables[2] = strdup("install");
    passables[3] = strdup(comet_dir);
    passables[4] = strdup(pm->name);
    passables[5] = strdup(pm->repo);
    passables[6] = strdup(pm->uri);
    passables[7] = strdup(pm->latest->string);
    passables[8] = 0;

    status = run_pipe(script_path, &passables);

    if (status != 0) {
        goto cleanup;
    }

    status = 1;

cleanup:
    free(passables[0]);
    free(passables[1]);
    free(passables[2]);
    free(passables[3]);
    free(passables[4]);
    free(passables[5]);
    free(passables[6]);
    free(passables[7]);
    free(passables[8]);
    free(passables);
    return status;
}

int install(Package* packages,
            const char* comet_dir,
            char* package,
            const char* index_path,
            int row_length) {
    return 1;
}