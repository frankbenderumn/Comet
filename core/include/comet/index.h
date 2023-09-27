/**
 * @file index.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief primary commands that are run in main.cc and comet executable
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_COMET_INDEX_H_
#define COMET_COMET_INDEX_H_

#include <string>
#include <vector>

#include "comet/util/common.h"
#include "comet/commands.h"

/**
 * @brief CometIndex class responsible for writing and reading binary index
 * of installed packages
 * 
 */
class CometIndex {
    /**
     * @brief absolute path to the environment index
     * 
     */
    std::string _index_path;
    
    /**
     * @brief index file uses fixed header. This variable stores the parsed
     * length of that header
     * 
     */
    int row_length = 0;

  public:
    /**
     * @brief a vector of InstalledPackages loaded from environment index file
     * 
     */
    std::vector<InstalledPackage*> installed;

    /**
     * @brief a vector of all created environments
     * 
     */
    std::vector<std::string> envs;
    
    /**
     * @brief current environment being used
     * 
     */
    std::string env;

    /**
     * @brief Construct a new Comet Index object
     * 
     * @param comet_dir the value of the COMET_DIR env param declared in
     * ./config/settings
     */
    CometIndex(const std::string& comet_dir);

    /**
     * @brief Destroy the Comet Index object. destroys vector of installed
     * packages too.
     * 
     */
    ~CometIndex();

    /**
     * @brief installs the inputted package if exists and writes metadata to
     * index
     * 
     * @param p the package to install
     */
    void install(Package* p);

    /**
     * @brief updates the existing package with the newer package information
     * 
     * @param p the package object to update the old package with
     */
    void update(Package* p);

    /**
     * @brief Create a ledger file to store list of installed directories and
     * files with their paths so they may be purged or uninstalled appropriately
     * 
     * @param ip the installed package to create ledger of
     */
    void create_ledger(InstalledPackage* ip);

    /**
     * @brief removes the files and directories within the ledger file then
     * removes the ledger
     * 
     * @param name the package to remove
     */
    void destroy_ledger(const std::string& name);

    /**
     * @brief after installing the package this function proceeds to add the 
     * installed package metadata to the environment index
     * 
     * @param ip the installed package metadata to add
     */
    void add(InstalledPackage* ip);

    /**
     * @brief purges the requested package from the current environment. Works
     * similarly to uninstall/remove but removes the build and source code as
     * well.
     * 
     * @param comet_dir value of the COMET_DIR environment variable
     * @param name the package to purge.
     */
    void purge(const char* comet_dir, const std::string& name);

    /**
     * @brief uninstalls the specified package from the user's current
     * environment. Does not remove source code, build or ledger.
     * 
     * @param name the package to uninstall
     */
    void remove(const std::string& name);

    /**
     * @brief Retrieves the package from index by name if it exists
     * 
     * @param name the package to retrieve
     * @return InstalledPackage* the object representation of the installed
     * package's metadata from the environment index
     */
    InstalledPackage* get_by_name(const std::string& name);

    /**
     * @brief lists all packages that have been installed in the environment
     * 
     */
    void list();

    /**
     * @brief loads the environment metadata, current environment and creates
     * InstalledPackage* objects of the packages within the environment
     * 
     */
    void load_env();

    /**
     * @brief loads the current environment index file to create
     * InstalledPackage* objects
     * 
     * @param index_path 
     * @return int status code of code execution
     */
    int load(const std::string& index_path);

};

#endif