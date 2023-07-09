#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "comet/io.h"
#include "comet/color.h"
#include "comet/index.h"
#include "comet/package.h"
#include "comet/setup.h"
#include "comet/manifest.h"

void package_array_destruct(Package** arr, int sz) {
    for (int i = 0; i < sz; i++) {
        if (arr[i]->manifest->latest) free(arr[i]->manifest->latest);
        if (arr[i]->manifest) free(arr[i]->manifest);
        if (arr[i]) free(arr[i]);
        // if (arr[i]->deps) char_arr_free(arr[i]->deps, arr[i]->deps_sz);
        // if (arr[i]->versions) char_arr_free(arr[i]->versions, arr[i]->versions_sz);
    }
    free(arr);
}


int package_serialize(Package* package, bytes_t data, size_t* row_length) {
    YEL("Serializing package... \n");

    MemMap attrs[] = {
        {package->manifest->name, COMET_STR, sizeof(package->manifest->name)},
        {package->manifest->source, COMET_STR, sizeof(package->manifest->source)},
        {package->manifest->uri, COMET_STR, sizeof(package->manifest->uri)},
        {package->manifest->repo, COMET_STR, sizeof(package->manifest->repo)},
        {package->manifest->latest->string, COMET_STR, sizeof(package->manifest->latest->string)},
        {&package->created_at, COMET_TS, sizeof(package->created_at)},
        {&package->updated_at, COMET_TS, sizeof(package->updated_at)},
        {&package->manifest->versions_sz, COMET_INT, sizeof(package->manifest->versions_sz)},
        {&package->manifest->deps_sz, COMET_INT, sizeof(package->manifest->deps_sz)},
        {package->signature, COMET_STR, sizeof(package->signature)}
    };

    int lando = sizeof(attrs) / sizeof(MemMap);
    BRED("Lando is: %i\n", lando);

    // Calculate the total size needed for serialization
    int sz = 0;
    for (int i = 0; i < lando; i++) {
        sz += attrs[i].size;
    }
    *row_length = sz;

    int offset = 0;
    for (int i = 0; i < lando; i++) {
        if (attrs[i].addr == NULL) {
            BRED("Null address encountered!\n");
            return -1;
        }
        memcpy(data + offset, attrs[i].addr, attrs[i].size);
        offset += attrs[i].size;
    }

    GRE("Created_at: %li\n", package->created_at);
    GRE("Updated_at: %li\n", package->updated_at);

    GRE("Package serialized!\n");

    return 0;
}

/**
 * WARN: Need to pass in package_dir to package_create
 */
Package* package_create(PackageManifest* pm) {
    Package* package = (Package*)malloc(sizeof(Package));
    if (package != NULL) {
        package->manifest = pm;
        package->created_at = time(NULL);
        package->updated_at = time(NULL);
        strncpy(package->signature, "123ds798aweiu123a23", sizeof(package->signature) - 1);
        package->signature[sizeof(package->signature) - 1] = '\0';
        package->row_id = -1;
    }
    return package;
}

void package_show_verbose(Package* package) {
    size_t max_length = 32;
    char created_at[32];
    char updated_at[32];
    printf("-----------------------------\n");
    printf("%-16s: %s\n", "name", package->manifest->name);
    struct tm* timeinfo = localtime(&(package->created_at));
    strftime(created_at, max_length, "%a, %b %d, %Y @ %I:%M:%S %p", timeinfo);
    printf("%-16s: %s\n", "created at", created_at);
    struct tm* timeinfo2 = localtime(&(package->updated_at));
    strftime(updated_at, max_length, "%a, %b %d, %Y @ %I:%M:%S %p", timeinfo2);
    printf("%-16s: %s\n", "updated at", updated_at);
    printf("%-16s: %s\n", "version", package->manifest->latest->string);
    printf("%-16s: %s\n", "signature", package->signature);
    printf("%-16s: %i\n", "rowid", package->row_id);
    printf("%-16s: %s\n", "uri", package->manifest->uri);
    printf("%-16s: %s\n", "source", package->manifest->source);
    printf("%-16s: %i\n", "versions", package->manifest->versions_sz);
    printf("%-16s: %i\n", "deps", package->manifest->deps_sz);
}


void package_show(Package* package) {
    size_t max_length = 32;
    char updated_at[32];
    struct tm* timeinfo = localtime(&(package->updated_at));
    strftime(updated_at, max_length, "%a, %b %d, %Y @ %I:%M:%S %p", timeinfo);
    printf("%-16s: %-32s\n", package->manifest->name, updated_at);
}

void packages_list_verbose(Package** packages, int packages_sz) {
    for (int i = 0; i < packages_sz; i++) {
        if (packages[i]) package_show_verbose(packages[i]);
    }
}

void packages_list(Package** packages, int packages_sz) {
    for (int i = 0; i < packages_sz; i++) {
        if (packages[i]) package_show(packages[i]);
    }
}