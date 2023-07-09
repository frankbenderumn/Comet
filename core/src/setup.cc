#include "comet/setup.h"

#include <dirent.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>

void char_arr_free(char** arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i]) free(arr[i]);
    }
    free(arr);
}

/** path restriction needed. i.e. do not allow /usr or ../ */
int run_pipe(const char* script, char*** args) {
    pid_t pid;
    int status;

    if (*args == NULL) {
        BRED("Should not happen!\n");
        return -1;
    }

    char** shit = *args;
    int lando = (sizeof(shit)/sizeof(char*));
    // printf("Here we go again!: %i\n", lando);

    // size_t len = sizeof(*args) / sizeof((*args)[0]);

    // printf("Array length is: %i\n", len(*args));

    /* Get and print my own pid, then fork and check for errors */
    // printf("My PID is %d\n", getpid());
    if ( (pid = fork()) == -1 ) {
        perror("Can't fork");
        return -1;
    }
    if (pid == 0) {
        if ( execv(script, *args) ) {
            perror("Can't exec");
            RED("%s\n", script);
            return -1;
        }
    } else if (pid > 0) {
        // CYA("Forked!\n");
        /* In the parent we must wait for the child
            to exit with waitpid(). Afterward, the
            child exit status is written to 'status' */
        waitpid(pid, &status, 0);
        // printf("Child executed with PID %d\n", pid);
        // printf("Its return status was %d\n", status);
        // printf("Its return status was %d\n", status / 256);
        if (WIFEXITED(status)) {
            // may need to use signals for IPC :(
            if (status / 256 == 0) {
                // BGRE("Subprocess terminated normally!\n");
            } else if (status == 11) {
                BRED("Segfault detected in subprocess!\n");
                // return -1;
                // exit(EXIT_FAILURE);
            } else {
                // BRED("Subprocess did not terminate normally! Exited with code: %i\n", status / 256);
                // return -1;
                // exit(EXIT_FAILURE);
            }
        }

    } else {
        fprintf(stderr, "Something went wrong forking\n");
        return -1;
    }
    return status / 256;
}

int parse_flags(int argc, char** args, char*** flags, char*** targets, int* num_targets) {
    int flags_sz = 0;
    for (int i = 1; i < argc; i++) {
        if (args[i][0] == '-') {
            if (flags_sz == 0) {
                (*flags) = (char**)calloc(1, sizeof(char*));
                char* flag = (char*)malloc(strlen(args[i]) + 1);
                strncpy(flag, args[i], strlen(args[i]));
                flag[strlen(args[i])] = '\0';
                (*flags)[0] = flag;
            } else {
                (*flags) = (char**)realloc(flags, sizeof(char*) * (flags_sz + 1));
                char* flag = (char*)malloc(strlen(args[i]) + 1);
                strncpy(flag, args[i], strlen(args[i]));
                flag[strlen(args[i])] = '\0';
                (*flags)[flags_sz] = flag;
            }
            flags_sz++;
        } else {
            (*num_targets)++;
            BLU("Num targets: %i\n", (*num_targets));
            if (*num_targets == 1) {
                *targets = (char**)realloc(*targets, sizeof(char*));
            } else {
                *targets = (char**)realloc(*targets, sizeof(char*) * *num_targets);
            }
            char* temp = (char*)malloc(strlen(args[(*num_targets)]) + 1);
            strncpy(temp, args[(*num_targets)], strlen(args[(*num_targets)]));
            temp[strlen(args[(*num_targets)])] = '\0';
            (*targets)[*num_targets] = temp;
        }
    }
    return flags_sz;
}

// int pipe(const char* script, char*** args) {
//     BGRE("=================================\n");
//     BGRE("              Piping             \n");
//     BGRE("=================================\n");

//     pid_t pid;
//     int status;

//     if (*args == NULL) {
//         return -1;
//     }

//     char** shit = *args;
//     int lando = (sizeof(shit)/sizeof(char*));
//     printf("Here we go again!: %i\n", lando);

//     // size_t len = sizeof(*args) / sizeof((*args)[0]);

//     printf("Array length is: %i\n", len(*args));

//     int null_fd = open("/dev/null", O_WRONLY);
//     if (null_fd < 0) {
//         perror("open");
//         return -1;
//     }

//     int saved_stdout = dup(STDOUT_FILENO);
//     if (saved_stdout < 0) {
//         perror("dup");
//         return -1;
//     }

//     if (dup2(null_fd, STDOUT_FILENO) < 0) {
//         perror("dup2");
//         return -1;
//     }
//     /* Get and print my own pid, then fork and check for errors */
//     // printf("My PID is %d\n", getpid());
//     if ( (pid = fork()) == -1 ) {
//         perror("Can't fork");
//         if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
//             perror("dup2");
//             return -1;
//         }

//         close(saved_stdout);
//         close(null_fd);
//         return -1;
//     }
//     if (pid == 0) {
//         if ( execv(script, *args) ) {
//             perror("Can't exec");
//             if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
//                 perror("dup2");
//                 return -1;
//             }

//             close(saved_stdout);
//             close(null_fd);
//             return -1;
//         }
//     } else if (pid > 0) {
//         /* In the parent we must wait for the child
//             to exit with waitpid(). Afterward, the
//             child exit status is written to 'status' */
//         waitpid(pid, &status, 0);
//         // printf("Child executed with PID %d\n", pid);
//         // printf("Its return status was %d\n", status);
//         // printf("Its return status was %d\n", status / 256);
//         if (WIFEXITED(status)) {
//             // may need to use signals for IPC :(
//             if (status / 256 == 0) {
//                 // BGRE("Subprocess terminated normally!\n");
//             } else if (status == 11) {
//                 BRED("Segfault detected in subprocess!\n");
//                 // exit(EXIT_FAILURE);
//             } else {
//                 BRED("Subprocess did not terminate normally!\n");
//                 // exit(EXIT_FAILURE);
//             }
//         }

//     } else {
//         fprintf(stderr, "Something went wrong forking\n");
//         if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
//             perror("dup2");
//             return -1;
//         }

//         close(saved_stdout);
//         close(null_fd);
//         return -1;
//     }

//     if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
//         perror("dup2");
//         return -1;
//     }

//     close(saved_stdout);
//     close(null_fd);

//     return status;
// }

void copy_dir(const char* source, const char* destination) {
    // Open the source directory
    DIR* dir = opendir(source);
    if (dir == NULL) {
        printf("Failed to open the source directory.\n");
        return;
    }

    // Create the destination directory
    int result = mkdir(destination, 0700);
    if (result != 0) {
        printf("Failed to create the destination directory.\n");
        return;
    }

    // Read the source directory
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Create the source and destination paths
        char sourcePath[1024];
        snprintf(sourcePath, sizeof(sourcePath), "%s/%s", source, entry->d_name);

        char destinationPath[1024];
        snprintf(destinationPath, sizeof(destinationPath), "%s/%s", destination, entry->d_name);

        // Get the file information
        struct stat st;
        stat(sourcePath, &st);

        // Recursively copy subdirectories
        if (S_ISDIR(st.st_mode)) {
            copy_dir(sourcePath, destinationPath);
        }
        // Copy regular files
        else {
            FILE* sourceFile = fopen(sourcePath, "rb");
            FILE* destinationFile = fopen(destinationPath, "wb");

            if (sourceFile != NULL && destinationFile != NULL) {
                // Copy file contents
                char buffer[4096];
                size_t bytesRead;
                while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
                    fwrite(buffer, 1, bytesRead, destinationFile);
                }

                fclose(sourceFile);
                fclose(destinationFile);
            }
        }
    }

    closedir(dir);
}

int comet_setup(const char* comet_dir) {

    char internal_dir[strlen(comet_dir) + strlen("/.internal") + 1];
    char cache_dir[strlen(comet_dir) + strlen("/.internal/cache") + 1];
    char package_dir[strlen(comet_dir) + strlen("/.internal/package") + 1];
    char registry_dir[strlen(comet_dir) + strlen("/.internal/registry") + 1];
    char settings_file[strlen(comet_dir) + strlen("/.internal/config/settings") + 1];
    char tokens_file[strlen(comet_dir) + strlen("/.internal/config/tokens") + 1];
    char env_dir[strlen(comet_dir) + strlen("/env") + 1];
    char config_dir[strlen(comet_dir) + strlen("/.internal/config") + 1];

    memcpy(internal_dir, comet_dir, strlen(comet_dir));
    memcpy(internal_dir + strlen(comet_dir), "/.internal", strlen("/.internal"));
    internal_dir[strlen(comet_dir) + strlen("/.internal")] = 0;

    memcpy(cache_dir, comet_dir, strlen(comet_dir));
    memcpy(cache_dir + strlen(comet_dir), "/.internal/cache", strlen("/.internal/cache"));
    cache_dir[strlen(comet_dir) + strlen("/.internal/cache")] = 0;

    memcpy(package_dir, comet_dir, strlen(comet_dir));
    memcpy(package_dir + strlen(comet_dir), "/.internal/package", strlen("/.internal/package"));
    package_dir[strlen(comet_dir) + strlen("/.internal/package")] = 0;

    memcpy(registry_dir, comet_dir, strlen(comet_dir));
    memcpy(registry_dir + strlen(comet_dir), "/.internal/registry", strlen("/.internal/registry"));
    registry_dir[strlen(comet_dir) + strlen("/.internal/registry")] = 0;

    memcpy(config_dir, comet_dir, strlen(comet_dir));
    memcpy(config_dir + strlen(comet_dir), "/.internal/config", strlen("/.internal/config"));
    config_dir[strlen(comet_dir) + strlen("/.internal/config")] = 0;

    memcpy(settings_file, comet_dir, strlen(comet_dir));
    memcpy(settings_file + strlen(comet_dir), "/.internal/config/settings", strlen("/.internal/config/settings"));
    settings_file[strlen(comet_dir) + strlen("/.internal/config/settings")] = 0;

    memcpy(tokens_file, comet_dir, strlen(comet_dir));
    memcpy(tokens_file + strlen(comet_dir), "/.internal/config/tokens", strlen("/.internal/config/tokens"));
    tokens_file[strlen(comet_dir) + strlen("/.internal/config/tokens")] = 0;

    memcpy(env_dir, comet_dir, strlen(comet_dir));
    memcpy(env_dir + strlen(comet_dir), "/env", strlen("/env"));
    env_dir[strlen(comet_dir) + strlen("/env")] = 0;
    

    if (create_dir(comet_dir) < 0) {
        BRED("Failed to create comet directory '%s'!\n", comet_dir);
        return -1;
    }

    if (create_dir(internal_dir) < 0) {
        BRED("Failed to create internal directory '%s'!\n", internal_dir);
        return -1;
    }

    if (create_dir(cache_dir) < 0) {
        BRED("Failed to create internal cache directory!\n");
        return -1;
    }

    if (create_dir(package_dir) < 0) {
        BRED("Failed to create internal package directory!\n");
        return -1;
    }

    // if (create_dir(registry_dir) < 0) {
    //     BRED("Failed to create internal package directory!\n");
    //     return -1;
    // }

    if (create_dir(env_dir) < 0) {
        BRED("Failed to create internal package directory!\n");
        return -1;
    }

    if (create_dir(config_dir) < 0) {
        BRED("Failed to create internal package directory!\n");
        return -1;
    }

    copy_dir("./registry", registry_dir);

    // FILE* fp = fopen(settings_file, "a");

    // if (fp == NULL) {
    //     BRED("Failed to create settings file!\n");
    // }

    // fclose(fp);

    FILE* fp = fopen(tokens_file, "a");

    if (fp == NULL) {
        BRED("Failed to create settings file!\n");
    }

    fclose(fp);

    return 0;
}

int verify_dirs() {
    if (!exists("/usr/local/comet")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/source")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/include")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/lib")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/build")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/compress")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/env")) {
        BRED("Error initing comet dirs\n"); return -1;
    }


    return 0;
}
