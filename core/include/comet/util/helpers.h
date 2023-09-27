/**
 * @file helpers.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief Amalgamation of various helper functions used throughout codebase to
 * help streamline code functionality
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_HELPERS_H_
#define COMET_HELPERS_H_

#include <string>

/**
 * @brief interpolates sensitive environment variables into string to assist
 * package retrieval and installation
 * 
 * @param uri the original uri passed as reference
 * @param name package name
 * @param version version of the requested package
 * @return int 
 */
int uri_env(std::string& uri, const char* name, const char* version);

/**
 * @brief parses arguments provided in command line and organizes them into
 * various categories
 * 
 * @param argc Number of arguments
 * @param args Argument array
 * @param flags Flags denote with - or -- prefix
 * @param targets Commands to run in executable
 * @param num_targets Number of commands/targets provided
 * @return int 
 */
int parse_flags(
    int argc,
    char** args,
    char*** flags,
    char*** targets,
    int* num_targets
);

/**
 * @brief Forks current process and runs necessary bash scripts for installation
 * and download of software packages
 * 
 * @param script Bash script to run
 * @param args Arguments to be passed to bash script
 * @return int 
 */
int run_pipe(const char* script, char*** args);

/**
 * @brief Removes shared prefix from a string if present
 * 
 * @param str String to compare prefix with
 * @param prefix Prefix to remove
 * @return char* Allocated string without prefix, needs to be freed.
 */
char* remove_common_prefix(const char *str, const char *prefix);

#endif