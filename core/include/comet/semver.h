/**
 * @file semver.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief Comparison and manipulation of semantic versionings
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_SEMVER_H_
#define COMET_SEMVER_H_

#include <sstream>

/**
 * @brief Handles manipulation and comparison of semantic versioning protocols
 * 
 */
struct SemVer {
    
    /**
     * @brief Major version number i.e. 1.x.x
     * 
     */
    int major = -1;
    
    /**
     * @brief Minor version number i.e. x.1.x
     * 
     */
    int minor = -1;
    
    /**
     * @brief Patch version number i.e. x.x.1
     * 
     */
    int patch = -1;

    /**
     * @brief string representation of semantic version (DEPRECATED?)
     * 
     */
    std::string string;
    
    /**
     * @brief Construct a new default SemVer object
     * 
     */
    SemVer() {}

    /**
     * @brief Construct a new SemVer object
     * 
     * @param version String representation of version number i.e. "1.7.10".
     */
    SemVer(const std::string& version);

    /**
     * @brief Converts the SemVer object to a stringified format (serializer).
     * 
     * @return std::string 
     */
    std::string stringify();

    /**
     * @brief Compares existing SemVer objects. Prime opportunity for operator
     * overloading but not utilized within the Alpha release, yet.
     * 
     * @param ext Secondary SemVer object to compare with.
     * @return int Similar to strcmp returns integer based upon comparator's
     * relation
     */
    int compare(SemVer* ext);

};

#endif