#include "comet/commands.h"
#include "comet/comet_manager.h"

#include <string.h>

int comet_pipe(Package* pm, const char* command) {
    DLOGC("yel", "comet_pipe: Installing %s...\n",
        static_cast<std::string*>(pm->attrs["name"]->addr)->c_str()
    );

    // retrieves environment package script path to string that can be passed 
    // to pipe
    char* comet_dir = getenv("COMET_DIR");
    size_t script_path_sz = strlen(comet_dir) +
        strlen("/.internal/manifest/script/") +
        strlen(static_cast<std::string*>(pm->attrs["source"]->addr)->c_str());
    char script_path[script_path_sz + 1];
    snprintf(script_path,
            script_path_sz + 1,
            "%s%s%s",
            comet_dir,
            "/.internal/manifest/script/",
            static_cast<std::string*>(pm->attrs["source"]->addr)->c_str()
    );
    script_path[script_path_sz] = 0;
    int status;

    // can dry up maybe?
    // reads the envfile to get active/currently used environment
    // remember to remove \n at the end of name
    std::string envfile_path = std::string(comet_dir) + "/envfile";
    std::string env = fread(envfile_path.c_str());
    env.pop_back();

    DLOGC("blu", "install.h::comet_pipe: ENV IS: %s (%li)\n", env.c_str(), env.size());
    DLOGC("blu", "install.h::comet_pipe: command: %s\n", command);

    // creates array of str arguments to be passed to bash script
    char** passables = (char**)malloc(sizeof(char*) * 10);
    passables[0] = strdup("sh");
    passables[1] = strdup(script_path);
    passables[2] = strdup(command);
    passables[3] = strdup(comet_dir);
    passables[4] = strdup(
        static_cast<std::string*>(pm->attrs["name"]->addr)->c_str());
    passables[5] = strdup(
        static_cast<std::string*>(pm->attrs["repo"]->addr)->c_str());
    passables[6] = strdup(
        static_cast<std::string*>(pm->attrs["uri_interpolated"]->addr)->c_str()
    );
    passables[7] = strdup(
        static_cast<std::string*>(pm->attrs["latest_str"]->addr)->c_str());
    passables[8] = strdup(env.c_str());
    passables[9] = 0;

    // runs the actual pipe using fork, etc.
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
    free(passables[9]);
    free(passables);
    return status;
}

void install(Comet* comet, const std::string& name) {
    Package* package = comet->manifest->get_by_name(name);
    InstalledPackage* ip = comet->index->get_by_name(name);

    if (!package) {
        BRED("Package '%s' does not exist!\n", name.c_str());
        return;
    }

    if (package->deps.size() > 0) {
        for (const auto& dep : package->deps) {
            DLOG("install: Dependency: %s\n", dep.c_str());
            Package* d = comet->manifest->get_by_name(dep);
            if (!d) {
                BRED("install: Dependency %s for package %s is not in manifest!\n",
                    dep.c_str(), package->name().c_str());
            } else {
                DLOGC("gre", "install: Dependency found!: %s\n", d->name().c_str());
                InstalledPackage* ip = comet->index->get_by_name(d->name());
                if (!ip) {
                    install(comet, dep);
                } else {
                    DLOG("Dependency %s already written into index.\n", dep.c_str());
                }
            }
        }
    }
    // verify package manifest exists
    if (package) {
        // verify not already installed
        if (!ip) {

            // download
            int status = comet_pipe(package, "get");
            
            // install
            if (package->source() == "github-clone") { 
                if (status == 1) {
                    BRED("Failed to retrieve package '%s'. " \
                        "Script was unsuccessful!\n", package->name().c_str());
                    return;
                }
            }

            status = comet_pipe(package, "install");

            comet->install(package);
        } else {
            BRED("Package '%s' already installed!\n", name.c_str());
        }
    } else {
        BRED("install: Failed to find package!\n");
    }
}