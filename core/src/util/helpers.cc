#include "comet/util/helpers.h"
#include "comet/util/color.h"

#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <regex.h>

int uri_env(std::string& uri, const char* name, const char* version) {
    regex_t regex;
    int ret;

    // Compile the regular expression
    ret = regcomp(&regex, "\\$\\{[^}]+\\}", REG_EXTENDED);
    if (ret) {
        printf("Failed to compile the regular expression.\n");
        return -1;
    }

    // Match the regular expression against the string
    regmatch_t match;

     // Create a mutable copy of the URI string
    char* mutable_uri = strdup(uri.c_str());

    while (regexec(&regex, mutable_uri, 1, &match, 0) == 0) {
        
        // Get the matched substring
        char* matched_substring = strndup(
            &mutable_uri[match.rm_so], match.rm_eo - match.rm_so
        );

        char env_var[strlen(matched_substring) - 3 + 1];
        strncpy(env_var, matched_substring + 2, strlen(matched_substring) - 3);
        env_var[strlen(matched_substring) - 3] = 0;

        // Perform the substitution
        // Replace the ${VERSION} tag with the desired value
        char* replacement = NULL;
        char* comet_dir = getenv("COMET_DIR");
        if (comet_dir == NULL) {
            BRED("No COMET_DIR environment\n");
            regfree(&regex);
            free(mutable_uri);
            free(matched_substring);
            return -1;
        }

        if (strcmp(env_var, "GET") == 0) {
            replacement = (char*)malloc(24 + strlen(comet_dir) + 1);
            strncpy(replacement, comet_dir, strlen(comet_dir));
            strncpy(
                replacement + strlen(comet_dir), "/.internal/manifest/.get", 24
            );
            replacement[24 + strlen(comet_dir)] = 0;
        } else if (strcmp(env_var, "VERSION") == 0) {
            replacement = strdup(version);
        } else {
            char* env_value = getenv(env_var);
            if (env_value == NULL) {
                // printf("No replacement found for URI interpolation!\n");
                free(mutable_uri);
                free(matched_substring);
                regfree(&regex);
                return -1;
            }
            replacement = strdup(env_value);
        }

        if (replacement == NULL) {
            // RED("No replacement found for uri interpolation!\n");
            regfree(&regex);
            free(mutable_uri);
            free(matched_substring);
            return -1;
        }
        
        size_t new_uri_sz = strlen(replacement) +
            match.rm_so + strlen(mutable_uri) - match.rm_eo;
        char new_uri[new_uri_sz + 1];
        memcpy(new_uri, mutable_uri, (long)match.rm_so);
        memcpy(new_uri + match.rm_so, replacement, strlen(replacement));
        memcpy(new_uri + match.rm_so + strlen(replacement),
            mutable_uri + match.rm_eo, strlen(mutable_uri) - match.rm_eo);
        new_uri[new_uri_sz] = 0;

        mutable_uri = (char*)realloc(mutable_uri, strlen(new_uri) + 1);
        strncpy(mutable_uri, new_uri, strlen(new_uri));
        mutable_uri[new_uri_sz] = 0;

        if (replacement) {
            free(replacement);
        }

        // Free the memory allocated for the matched substring
        free(matched_substring);
    }

    // Clean up the regular expression
    regfree(&regex);

    uri = std::string(mutable_uri);

    // Free the memory allocated for the mutable URI
    free(mutable_uri);

    return 1;
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

    /* Get and print my own pid, then fork and check for errors */
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
        /* In the parent we must wait for the child
            to exit with waitpid(). Afterward, the
            child exit status is written to 'status' */
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            if (status / 256 == 0) {
            } else if (status == 11) {
                BRED("Segfault detected in subprocess!\n");
            } else {
            }
        }

    } else {
        fprintf(stderr, "Something went wrong forking\n");
        return -1;
    }
    return status / 256;
}

int parse_flags(int argc, char** args, char*** flags, char*** targets,
                int* num_targets) {
    
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
                (*flags) = (char**)realloc(
                    flags, sizeof(char*) * (flags_sz + 1)
                );
                char* flag = (char*)malloc(strlen(args[i]) + 1);
                strncpy(flag, args[i], strlen(args[i]));
                flag[strlen(args[i])] = '\0';
                (*flags)[flags_sz] = flag;
            }
            flags_sz++;
        } else {
            (*num_targets)++;
            if (*num_targets == 1) {
                *targets = (char**)calloc(1, sizeof(char*));
            } else {
                *targets = (char**)realloc(
                    *targets, sizeof(char*) * *num_targets
                );
            }
            char* temp = (char*)malloc(strlen(args[i]) + 1);
            strncpy(temp, args[i], strlen(args[i]));
            temp[strlen(args[i])] = '\0';
            (*targets)[(*num_targets) - 1] = temp;
        }
    }
    return flags_sz;
}

char* remove_common_prefix(const char *str, const char *prefix) {
    size_t prefixLen = strlen(prefix);
    size_t strLen = strlen(str);

    // Check if 'str' starts with 'prefix'
    if (strLen >= prefixLen && strncmp(str, prefix, prefixLen) == 0) {
        // Calculate the length of the remaining part of 'str'
        size_t resultLen = strLen - prefixLen + 1; // +1 for the null terminator

        // Allocate memory for the result string
        char *result = (char *)malloc(resultLen);

        // Copy the remaining part of 'str' to the result
        strcpy(result, str + prefixLen);

        return result;
    } else {
        // If 'str' doesn't start with 'prefix', return a copy of 'str'
        return strdup(str);
    }
}