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

#include "comet/defs.h"
#include "comet/io.h"
#include "comet/index.h"
#include "comet/command/commands.h"
#include "comet/setup.h"

int main(int argc, char* argv[]) {

    const char* COMET_DIR = getenv("COMET_DIR");

    if (COMET_DIR == NULL) {
        printf("\033[1;31mError:\033[0m comet dir environment variable not detected!\n");
        return 1; 
    }

    if (argc < 2) {
        printf("\033[1;31mError:\033[0m Invalid number of arguments! Please use comet --help for more options!\n");
        return -1;
    }

    char** flags = NULL;
    char** targets = NULL;
    char* OPTION = NULL; 
    char* COMMAND = NULL;
    int targets_sz = 0;
    int flags_sz = parse_flags(argc, argv, &flags, &targets, &targets_sz);

    if (targets == NULL) {
        printf("\033[1;31mError:\033[0m No targets found!\n");
        goto cleanup;
    }

    COMMAND = targets[0];
    YEL("Command: %s\n", COMMAND);
    YEL("Flags: %i\n", flags_sz);

    if (argc > 2) {
        OPTION = argv[2];
    }

    if (strcmp(COMMAND, "help") == 0) {
        printf("====================================================================================\n");
        printf("                                  Comet Package Manager                             \n");
        printf("====================================================================================\n");
        printf("%-32s: %s\n", "install", "Install a package within the registry");   
        printf("%-32s: %s\n", "uninstall", "Uninstall a package");   
        printf("%-32s: %s\n", "uninstall -a", "Uninstall all packages");   
        printf("%-32s: %s\n", "upgrade -a", "Uninstall all packages");   
        printf("%-32s: %s\n", "list", "List all packages with detailed information");   
        printf("%-32s: %s\n", "upload <script> <deps> <user>", "Upload a package to the registry \n\t\t\twith given dependencies for a user");   
        printf("%-32s: %s\n", "show <package_name>", "Get detailed information about a package");
        printf("%-32s: %s\n", "update <package_name>", "Update package with a new script");
        printf("%-32s: %s\n", "load", "Reads package names from a comet.txt file to install required\n\t\t\tpackages");
        printf("%-32s: %s\n", "purge <package>", "Removes a package and its cached build!");
        printf("%-32s: %s\n", "purge -a/--all", "(Not recommended) Will clear all package \n\t\t\tinstallations (cached and environmental). Will require password.\n");

    } else if (strcmp(COMMAND, "install") == 0) {

        if (argc < 3) {
            BRED("Invalid number arguments for COMMAND install!\n");
            goto cleanup;
        }

        install(targets[0]);

    } else if (strcmp(COMMAND, "update") == 0) {

        if (argc < 3) {
            BRED("Invalid number arguments for COMMAND install!\n");
            goto cleanup;
        }

        update(targets[0]);

    } else if (strcmp(COMMAND, "uninstall") == 0) {

        if (argc < 3) {
            BRED("Invalid number arguments for COMMAND install!\n");
            goto cleanup;
        }

        uninstall(targets[0]);

    } else if (strcmp(COMMAND, "show") == 0) {

        show(targets[0]);

    } else if (strcmp(COMMAND, "list") == 0) {

        bool verbose = false;
        for (int i = 0; i < flags_sz; i++) {
            MAG("-> %s\n", flags[i]);
            if (strcmp(flags[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(flags[i], "--verbose") == 0) {
                verbose = true;
            }
        }

        BMAG("VERBOSE: %i\n", verbose);

        list(verbose);

    } else if (strcmp(COMMAND, "purge") == 0) {

        purge(targets[0]);

    } else if (strcmp(COMMAND, "newuser") == 0) {
        BYEL("Creating new user...\n");
        if (argc != 3) {
            BRED("Invalid number of arguments provided for newuser!\n");
            goto cleanup;
        }

        char* newuser = argv[2];

    } else if (strcmp(COMMAND, "token") == 0) {
        
        if (argc != 4) {
            BRED("Invalid number of arguments provided for COMMAND 'token'. Usage: comet token <key> <value>\n");
            goto cleanup;
        }

        token(argv[2], argv[3]);

    } else if (strcmp(COMMAND, "update_token") == 0) {

        if (argc != 4) {
            BRED("Invalid number of arguments provided for COMMAND 'token'. Usage: comet update_token <key> <value>\n");
            goto cleanup;
        }

        update_token(argv[2], argv[3]);

    } else {
        BRED("Invalid COMMAND provided!\n");
    }



cleanup:
    BMAG("Cleanup!\n===========================\n");
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
    BGRE("Gracefully terminated!\n");
 
    return 0;
}