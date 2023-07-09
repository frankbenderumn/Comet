#include "comet/command/command.h"

int command(PackageManifest* pm, const char* command_str) {
    char* comet_dir = getenv("COMET_DIR");
    size_t script_path_sz = strlen(comet_dir) + strlen("/.internal/manifest/script/") + strlen(pm->source);
    char script_path[script_path_sz + 1];
    snprintf(script_path, script_path_sz + 1, "%s%s%s", comet_dir, "/.internal/manifest/script/", pm->source);
    script_path[script_path_sz] = 0;
    int status = -1;

    char** passables = (char**)malloc(sizeof(char*) * 9);
    passables[0] = strdup("sh");
    passables[1] = strdup(script_path);
    passables[2] = strdup(command_str);
    passables[3] = strdup(comet_dir);
    passables[4] = strdup(pm->name);
    passables[5] = strdup(pm->repo);
    passables[6] = strdup(pm->uri_interpolated);
    passables[7] = strdup(pm->latest->string);
    passables[8] = 0;

    status = run_pipe(script_path, &passables);

    if (status != 0) {
        if (status == 2) {
            printf("\033[1;31mError:\033[0m Package '%s' already downloaded!\n", pm->name);
        }
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