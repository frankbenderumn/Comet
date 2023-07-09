#ifndef COMET_PACKAGE_H_ 
#define COMET_PACKAGE_H_

#include <string>

#include "comet/defs.h"

typedef unsigned char* bytes_t;

/**
 * @brief Serializes the package into into a string of bytes and assigns 
 * the string and size to the references data and size, respectively
 * 
 * @param package the package to be serialized
 * @param data the references to bind the stringified package to
 * @param size the size of the stringified package
 * @return int success or failure TODO: change to type void?
 */
int package_serialize(Package* package, bytes_t* data, size_t* size);


/**
 * @brief Creates the package object and adds it the list of packages. This is 
 * essentially a c version constructor.
 * 
 * @param package the package object to construct
 * @param packages the list of packages to add the package too
 * @param name the name of the package
 * @param version the version string of the package
 * @param script the script to install the package
 * @param mmap WARN: not sure what mmap is for here
 * @param num_attrs WARN: number of attrs should clarify what this is for
 * @return int 
 */
Package* package_create(
    PackageManifest* pm
);

void package_array_destruct(Package** arr, int sz);


/** WARN: The two following functions are redundant */
/**
 * @brief dumps the package to terminal
 * 
 * @param package 
 */
void package_dump(Package* package);


/**
 * @brief shows the package to terminal
 * 
 * @param package 
 */
void package_show(Package* package);

/**
 * @brief Converts the package to a serializable string that will be written
 * to the index. This will be read out from the file as a Package object. The
 * difference between Package and PackageManifest is minor. The primary
 * distinction is that Package has timestamps and signatures where as the
 * manifest is general information relevant to retrieval.
 * 
 * @param p package to serialize
 * @param serialized the serialized byte string of the package manifest
 * @param size the length of the serialized byte string
 * @return status code
 */
int package_serialize(Package* p, unsigned char* serialized, size_t* size);

// int package_deserialize_all(Package*** packages);

void packages_list(Package** packages, int packages_sz);

void packages_list_verbose(Package** packages, int packages_sz);

#endif