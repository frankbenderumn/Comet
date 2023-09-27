/**
 * @file commands.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief primary commands that are run in main.cc and comet executable
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_COMMANDS_H_
#define COMET_COMMANDS_H_

#include <string>

#include "comet/comet_manager.h"

class Comet;

/**
 * @brief Loads specific versions of packages from a comet.lock file 
 * 
 * @param comet Comet object that contains index and manifest information
 * @param path Path of the comet.lock file
 */
void load(Comet* comet, char* path);

/**
 * @brief Shows currently used environment
 * 
 * @param comet_dir Value of the COMET_DIR enviornment variable
 */
void env_show(const char* comet_dir);

/**
 * @brief Shows currently used environment
 * 
 * @param comet_dir Value of the COMET_DIR enviornment variable
 * @param option Name of environment to create
 */
void env_create(const char* comet_dir, const char* option);

/**
 * @brief Creates new environment. Newly created environment becomes the current
 * environment.
 * 
 * @param comet_dir Value of the COMET_DIR enviornment variable
 * @param option Name of environment to use, if exists.
 */
void env_use(const char* comet_dir, const char* option);

/**
 * @brief Lists all enviornments created in user's system.
 * 
 * @param comet_dir Value of the COMET_DIR enviornment variable
 */
void env_list(const char* comet_dir);

/**
 * @brief Helper function for manipulating packages with scripts. TODO: 
 * This function may be redundant and not necessary
 * 
 * @param pm Package to run functionality on
 * @param command Command to run on Package
 * @return int status code
 */
int comet_pipe(Package* pm, const char* command);

/**
 * @brief Installs the specified package
 * 
 * @param comet Comet object that contains index and manifest information
 * @param package_name Package to install
 */
void install(Comet* comet, const std::string& package_name);

/**
 * @brief 
 * 
 * @param comet Comet object that contains index and manifest information
 * @param package_name Package to show
 */
void show(Comet* comet, const std::string& package_name);

/**
 * @brief Creates a new token as an environment variable
 * 
 * @param key Token key to create
 * @param val Value to assign to token
 */
void token(const char* key, const char* val);

/**
 * @brief Updates existing token in environment with new value
 * 
 * @param key Token key to update, if exists
 * @param val Value to update token with
 */
void update_token(const char* key, const char* val);

/**
 * @brief Loads tokens from a base file similar to comet load. Default file is
 * comet.tokens
 * 
 * @param tokens_path path of the file to load tokens from
 */
void token_load(const char* tokens_path);

#endif