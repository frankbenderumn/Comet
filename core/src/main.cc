#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

#include <string>

#include "comet/comet.h"

#define INVARG(argc, num, name) \
if (argc < num) { \
    BRED("Invalid number arguments for COMMAND %s!\n", name); \
    goto cleanup; \
}

/** TODO: DRY up argc check; convert to command pattern design */

int main(int argc, char* argv[]) {

    // retrieve COMET_DIR env var and ensure number of args is correct,
    // although argc check could belong to argparse functionality
    const char* COMET_DIR = getenv("COMET_DIR");

    if (COMET_DIR == NULL) {
        printf("\033[1;31mError:\033[0m comet dir environment " \
            "variable not detected!\n");
        return 1; 
    }

    if (argc < 2) {
        printf("\033[1;31mError:\033[0m Invalid number of arguments! " \
            "Please use comet --help for more options!\n");
        return 1;
    }

    // define import file paths and parse/load info through comet constructor
    std::string index_path = std::string(COMET_DIR);
    std::string manifest_path = "./manifest/manifest.json";
    Comet* comet = new Comet(index_path, manifest_path);
    DLOGC("gre", "Main: Initial loading of manifest and " \
        "index completed successfully!\n");

    // argparse declarations
    char** flags = NULL;
    std::vector<std::string> flag_vec;
    std::vector<std::string> target_vec;
    char** targets = NULL;
    char* OPTION = NULL; 
    char* COMMAND = NULL;
    int targets_sz = 0;

    // parse flags for argparse
    int flags_sz = parse_flags(argc, argv, &flags, &targets, &targets_sz);

    if (targets == NULL) {
        printf("\033[1;31mError:\033[0m No targets found!\n");
        goto cleanup;
    }

    COMMAND = targets[0];
    
    if (argc > 2) {
        OPTION = argv[2];
    }

    // logic for argparse, could use future improvements
    if (flags_sz > 0) {
        for (int i = 0; i < flags_sz; i++) {
            std::string s(flags[i]);
            if (s[0] == '-') { 
                flag_vec.push_back(s);
            } else {
                target_vec.push_back(s);
            }
        }
    } else if (argc > 3) {
        for (int i = 2; i < argc; i++) {
            std::string s(argv[i]);
            target_vec.push_back(s);
        }
    }

    DLOGC("blu", "Argparse\n")
    DLOGC("blu", "=========================================================\n");

    DLOG("COMMAND: %s\n", COMMAND);
    DLOG("TARGET: %s\n", OPTION);
    DLOG("TARGETS:\n");
    for (const auto& t : target_vec) {
        DLOG("\t%s\n", t.c_str());
    }
    DLOG("FLAGS:\n");
    for (const auto& f : flag_vec) {
        DLOG("\t%s\n", f.c_str());
    }
    DLOGC("blu", "---------------------------------------------------------\n");

    if (strcmp(COMMAND, "help") == 0) {

        printf("====================================================================================\n");
        printf("                                  Comet Package Manager                             \n");
        printf("====================================================================================\n");
        printf("%-32s: %s\n", "install <package>", "Install a package within the registry");   
        printf("%-32s: %s\n", "uninstall <package>", "Uninstall a package");   
        printf("%-32s: %s\n", "upgrade", "Uninstall all packages");   
        printf("%-32s: %s\n", "list", "List all packages with detailed information");   
        printf("%-32s: %s\n", "show <package>", "Get detailed information about a package");
        printf("%-32s: %s\n", "update <package>", "Update package with a new script");
        printf("%-32s: %s\n", "load <comet.lock path>", "Reads package names from a comet.lock file to install required packages");
        printf("%-32s: %s\n", "purge <package>", "Removes a package and its cached build!");
        printf("%-32s: %s\n", "token <key>?", "");
        printf("%-32s: %s\n", "token-update <key>?", "");
        printf("%-32s: %s\n", "env-create <environment> [comet.lock path]", "create a new environment and optionally load a comet.lock file");
        printf("%-32s: %s\n", "env-show <environment>", "show the contents of the provided environment");
        printf("%-32s: %s\n", "env-list", "list the names of all environments");
        printf("%-32s: %s\n", "env-use <environment>", "use the specified environment");
        printf("%-32s: %s\n", "purge -a/--all", "(Not recommended) Will clear all package installations (cached and environmental). Will require password.\n");

    } else if (strcmp(COMMAND, "install") == 0) {
        printf("Installing...\n");
        INVARG(argc, 3, "install");
        install(comet, OPTION);

    } else if (strcmp(COMMAND, "show") == 0) {
        INVARG(argc, 3, "show");
        printf("===========================================================\n");
        printf("                     %s                      \n", OPTION);
        printf("===========================================================\n");
        show(comet, OPTION);

    } else if (strcmp(COMMAND, "list") == 0) {
        comet->index->list();

    } else if (strcmp(COMMAND, "uninstall") == 0) {
        INVARG(argc, 3, "uninstall");
        comet->index->remove(OPTION);

    } else if (strcmp(COMMAND, "purge") == 0) {
        INVARG(argc, 3, "purge");
        comet->index->purge(COMET_DIR, OPTION);

    } else if (strcmp(COMMAND, "update") == 0) {
        INVARG(argc, 3, "update");
        comet->update(OPTION);

    } else if (strcmp(COMMAND, "token-update") == 0) {
        INVARG(argc, 4, "token-update");
        update_token(OPTION, targets[2]);

    } else if (strcmp(COMMAND, "token-load") == 0) {
        token_load(OPTION);

    } else if (strcmp(COMMAND, "token") == 0) {
        INVARG(argc, 4, COMMAND);
        token(OPTION, targets[2]);

    } else if (strcmp(COMMAND, "env-list") == 0) {
        INVARG(argc, 3, COMMAND);
        env_list(COMET_DIR);

    } else if (strcmp(COMMAND, "env-show") == 0) {
        INVARG(argc, 3, COMMAND);
        env_show(COMET_DIR);

    } else if (strcmp(COMMAND, "env-use") == 0) {
        INVARG(argc, 3, COMMAND);
        env_use(COMET_DIR, OPTION);

    } else if (strcmp(COMMAND, "env-create") == 0) {
        INVARG(argc, 3, COMMAND);
        env_create(COMET_DIR, OPTION);

    } else if (strcmp(COMMAND, "load") == 0) {
        load(comet, OPTION);

    } else if (strcmp(COMMAND, "upgrade") == 0) {
        /** TODO: upgrade command */

    } else if (strcmp(COMMAND, "upgrade") == 0) {
        /** TODO: restore */

    } else {
        BRED("Invalid COMMAND provided!\n");
    }

cleanup:
    DLOGC("mag", "Cleaning up...\n");
    if (targets) {
        for (int i = 0; i < targets_sz; i++) {
            if (targets[i]) free(targets[i]);
        }
        if (targets) free(targets);
    }
    if (flags) {
        for (int i = 0; i < flags_sz; i++) {
            if (flags[i]) free(flags[i]);
        }
        if (flags) free(flags);
    }
    delete comet;
    DLOGC("gre", "Gracefully terminated!\n");

    return 0;
}