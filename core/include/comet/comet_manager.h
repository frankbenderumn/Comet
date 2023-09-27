/**
 * @file comet_manager.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief Manages functionality for the comet manifest, packages and index
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */
#ifndef COMET_COMET_MANAGER_H_
#define COMET_COMET_MANAGER_H_

#include <string>

#include "comet/manifest.h"
#include "comet/index.h"

class CometIndex;

class Comet {
    /**
     * @brief absolute path of the current environment index
     * 
     */
    std::string _index_path;
    /**
     * @brief absolute path of the manifest.json file
     * 
     */
    std::string _manifest_path;

  public:
    /**
     * @brief CometManifest manifest
     * 
     */
    CometManifest* manifest = nullptr;
	
	/**
	 * @brief CometIndex pointer
	 * 
	 */
    CometIndex* index = nullptr;

    /**
     * @brief a vector of all installed packages as InstalledPackage objects
     * 
     */
    std::vector<InstalledPackage*> installed;

	/**
	 * @brief Construct a new Comet object
	 * 
	 * @param index_path absolute path to the current environment index
	 * @param manifest_path absolute path to the manifest json file
	 */
    Comet(const std::string& index_path, const std::string& manifest_path);

	/**
	 * @brief Destroy the Comet object
	 * 
	 */
    ~Comet();

	/**
	 * @brief wrapper to conveniently install package parsed from
	 * manifest file
	 * 
	 * @param package Package to install
	 */
    void install(Package* package);

	/**
	 * @brief wrapper to conveniently update package by name
	 * 
	 * @param package_name Package to update
	 */
    void update(const std::string& package_name);

};

#endif