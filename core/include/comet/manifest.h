/**
 * @file manifest.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief responsible for creating object representations (Package) of packages
 * available to download and install within manifest 
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_MANIFEST_H_
#define COMET_MANIFEST_H_

#include <stdio.h>

#include <fstream>
#include <string>
#include <sstream>

#include "picojson.h"

#include "comet/util/common.h"
#include "comet/package.h"

/**
 * @brief Manages I/O manipulation and installation information regarding
 * potential packages to install, but not actual installed packages.
 * CometManifest manages Package* objects which are unrealized packages.
 * 
 */
class CometManifest {
    /**
     * @brief absolute path of the comet manifest json file
     * 
     */
    std::string _manifest_path;
    
    /**
     * @brief vector of all packages within manifest represented as objects
     * 
     */
    std::vector<Package*> _packages;

  public:
    /**
     * @brief Construct a new Comet Manifest object
     * 
     * @param manifest_path Absolute path of the comet manifest.json file.
     * 
     */
    CometManifest(const std::string& manifest_path);

    /**
     * @brief Destroy the Comet Manifest object
     * 
     */
    ~CometManifest();

    /**
     * @brief load all unrealized (potential) packages into memory so that if
     * they are requested to be installed they can be readily installed.
     * 
     * @return int 
     *
     */
    int load();

    /**
     * @brief Get the by name object
     * 
     * @param name of the manifest object to retrieve ("data required to
     * install package")
     * 
     * @return Package* Encapsulation of necessaery needed to install requested
     * package
     *  
     */
    Package* get_by_name(const std::string& name);

};

#endif