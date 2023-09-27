/**
 * @file package.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief 
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

/**
 * Alpha Notes: Needs work on inheritance and template casting. For more robust
 * storage engine, schema declaration and parsing could be useful
 * 
 */

#ifndef COMET_PACKAGE_H_
#define COMET_PACKAGE_H_

#include <ctime>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

#include "comet/util/common.h"
#include "comet/semver.h"

/**
 * @brief enumerable to handle various types with the intent of providing type
 * flexibility and a mild use-case of weak typing to allow for more dynamic and
 * diverse schemas.
 * 
 */
enum CometAttrType {
    COMET_STR,
    COMET_TIMESTAMP
};

/**
 * @brief Wrapper for dealing with flexible type serialization
 * and deserialization in binary format.
 * 
 */
struct CometAttr {
    /**
     * @brief type independent memory allocation of data by byte and address;
     * 
     */
    void* addr;
    
    /**
     * @brief enumerable to manage associatively the type to convert the void*
     * addr variable
     * 
     */
    CometAttrType type;

    /**
     * @brief the size of the data stored to assist with binary file processing
     * 
     */
    size_t size;
    
    /**
     * @brief Construct a new CometAttr object
     * 
     * @param _addr Address of the initial allocation point for passed data
     * @param _type Type of the data passed as an enumerable. See CometAttrType.
     * @param _size Size of the store data; important for string types.
     */
    CometAttr(void* _addr, CometAttrType _type, size_t _size) :
                addr(_addr), type(_type), size(_size) {}
    
    /**
     * @brief Destroy the CometAttr object, deletion uses casting to ensure
     * appropriate deallocation of allocated data
     * 
     */
    ~CometAttr();
};

class InstalledPackage;

/**
 * @brief Responsible for parsing and storing in memory as class objects the 
 * contents, parameters, and values of the metadata associated and required in
 * the process of installing packages from various programatic sources.
 * 
 */
class Package {
  public:
    /**
     * @brief std::map does not quarantee insertion order without altering the 
     * hash algorithm, therefore this vector is used to preserve order.
     * 
     */
    std::vector<std::string> insertion_order;
    
    /**
     * @brief maps the key values to the corresponding CometAttr. This
     * generalization allows for more complext template-based functionality
     * in C++ and can mimic weak typed declarations present in languages like
     * Javascript and Python.
     * 
     */
    std::map<std::string, CometAttr*> attrs;
    
    /**
     * @brief list of various package dependencies stored in an in memory
     * vector format.
     * 
     */
    std::vector<std::string> deps;
    
    /**
     * @brief legacy support in package management is important, therefore a 
     * vector to store, in memory, potential versions will help with the
     * comet.lock file processing for specific version requests. 
     * 
     */
    std::vector<std::string> versions;

    /**
     * @brief Semantic version wrapper of version string
     * 
     */
    SemVer* version = nullptr;
    
    /**
     * @brief Default constructor
     * 
     */
    Package() {}
    
    /**
     * @brief Construct a new Package object
     * 
     * @param name Name of the Package
     * @param source Distinguishment of source, i.e. apt-get vs. github install
     * @param repo Reserved for github installs but required parameter. Stores
     * the name of the github repo otherwise value is 'N/A' if does not exist or
     * not needed
     * @param uri Https link of the source code to transfer over sockets.
     * @param latest_str Latest version of source code repository or download 
     */
    Package(const std::string& name,
            const std::string& source,
            const std::string& repo,
            const std::string& uri,
            const std::string& latest_str);

    /**
     * @brief Destroy the Package object. Removes all CometAttr and SemVer
     * objects as well
     * 
     */
    virtual ~Package();

    /**
     * @brief Installs the corresponding Package* object abiding by the manifest
     * constraints and returns an InstalledPackage* enhancement of the original
     * package
     * 
     * @return InstalledPackage* Object representation of the freshly installed
     * package
     * 
     */
    InstalledPackage* install();

    /**
     * @brief Dumps contents of object to terminal
     * 
     */
    virtual void dump();

    /**
     * @brief Retrieves the name of the installed Package.
     * 
     * @return const std::string Name of Package
     */
    const std::string name();

    /**
     * @brief Retrieves the version string of the installed Package
     * 
     * @return const std::string String representation of Package
     */
    const std::string latest();

    /**
     * @brief Serializes the Package into a binary-friendly format, not
     * currently implemented but will be implemented after inheritance issues
     * are resolved.
     * 
     * @return const std::string 
     */
    virtual const std::string serialize() const;

    /**
     * @brief Retrieves the source of the installed package. WARN: replication 
     * of Package functionality.
     * 
     * @return const std::string Source of Package
     */
    const std::string source();

};

/**
 * TODO: Alpha Release: utilize inheritance between Package and InstalledPackage
 */

/**
 * @brief Representation of a Package or manifest object that has been installed
 * within the user's current enviornment.
 * 
 */
class InstalledPackage {
    /**
     * @brief Currently implementing inheritance through composition.
     * 
     */
    Package* _package = nullptr;
  
  public:
    /**
     * @brief Duplication of Package declaration
     * 
     */
    size_t size = 0;
    
    /**
     * @brief Duplication of Package declaration
     * 
     */
    std::map<std::string, CometAttr*> attrs;
    
    /**
     * @brief Auto incremented assigned id to in-memory package instantiated
     * in the beginning of control flow when the index is parsed and loaded.
     * 
     */
    int row_id = -1;

    /**
     * @brief Pointer to SemVer object for easier version control and
     * comparison.
     * 
     */
    SemVer* version = nullptr;

    /**
     * @brief Construct a new Installed Package object
     * 
     * @param package Base Package object to extend and evolve
     */
    InstalledPackage(Package* package);
    
    /**
     * @brief Construct a new Installed Package object
     * 
     * @param name Package name
     * @param source Download source i.e. github, apt-get, ftp
     * @param repo Name of the repo, specific to github installation for now
     * @param uri Uri for the download link to retrieve source code locally.
     * @param latest_str Most recent or tagged version in string form.
     * @param created_at Timestamp of creation time
     * @param updated_at Timestamp of updation time
     * @param row_id Runtime id to assist with organization and iteration
     */
    InstalledPackage(const std::string& name,
                    const std::string& source,
                    const std::string& repo,
                    const std::string& uri,
                    const std::string& latest_str,
                    const time_t& created_at,
                    const time_t& updated_at,
                    const int& row_id
                );
    
    /**
     * @brief Destroy the Installed Package object. Destroys CometAttr's and
     * SemVer allocations
     * 
     */
    ~InstalledPackage();

    /**
     * @brief Retrieves the name of the installed package. WARN: replication 
     * of Package functionality
     * 
     * @return const std::string Name of Package
     */
    const std::string name();

    /**
     * @brief Retrieves the source of the installed package. WARN: replication 
     * of Package functionality.
     * 
     * @return const std::string Source of Package
     */
    const std::string source();
    
    /**
     * @brief Dumps contents of InstalledPackage. WARN: Potential replication
     * of Package functionality.
     * 
     */
    void dump();

    /**
     * @brief Terminal output. Nearly identical to dump functionality, but 
     * geared toward no Package encapsulated object. (aka Package*).
     * WARN: Potentially redundant only show is required. Can delete dump. 
     * 
     */
    void show();

    /**
     * @brief Package serializer to allow for writing of binary index file
     * 
     * @return const std::string Unsigned char binary representation of data to
     * write
     *  
     */
    const std::string serialize();

    /** TODO: should do through a schema file for deserialize function */

    /**
     * @brief Deserializes package by reading from environmental file and 
     * creating in memory object representations of the installed package's
     * metadata
     * 
     * @param buffer C buffer to write binary data to 
     * @param record_length Length of fixed header for each package
     * @param row_id Row id assigned to assist update and removal functionality.
     * @return InstalledPackage* 
     */
    static InstalledPackage* deserialize(
        const unsigned char* buffer,
        size_t record_length,
        int row_id
    );

};

#endif