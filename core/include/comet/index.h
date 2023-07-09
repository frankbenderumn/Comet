#ifndef COMET_INDEX_H_
#define COMET_INDEX_H_ 

#include "comet/defs.h"

/**
 * @brief projects certain attributes of the package specified in start and end
 * 
 * @param name_ct counts the names, WARN: specific to name project
 * @param index_path path of the comet index
 * @param row_length length of the serialized package byte string
 * @param start byte start offset for attribute to project
 * @param end byte end offset for attribute to project
 * @return char** returns list of name, WARN: specific to name project
 */
// char** index_project(
//     int* name_ct,
//     const char* index_path,
//     int row_length,
//     int start,
//     int end
// );

int index_ascii_write(
    PackageManifest* pm
);


/**
 * @brief adds to the index. WARN: unsure if add or create function. I see no
 * specific package object to add
 * 
 * @param packages the packages array
 * @param num_packages the number of packages
 * @param index_path the path for the comet index file
 * @param record_length the byte length of the serialized package
 * @return int returns < 0 if error encountered
 */
int index_add(
    Package*** packages,
    int packages_sz,
    PackageManifest* pm
);


/**
 * @brief dumps the list of packages by iterating package array.
 * WARN: may be redundant
 * 
 * @param packages the array of packages to iterate
 */
void index_dump(Package** packages, int packages_sz);

// int index_load(Package*** packages);

int index_remove(Package** packages, int packages_sz, char* name);

/**
 * @brief verifies the creattion and updation of the new package and indexed
 * package are consistent
 * 
 * @param new_package the new new package created 
 * @param index_package the package parsed out of the index
 * @param index_path the location of the comet index file
 * @return int returns < 0 if error encountered
 */
// int index_verify(
//     Package* new_package,
//     Package* index_package,
//     const char* index_path
// );


/**
 * @brief updates existing package in index with new package information 
 * 
 * @param p package to update
 * @param index_path path of the comet index
 * @param row_length byte length of the serialized package
 * @return int returns < 0 if error encountered
 */
// int index_package_update(Package* p, const char* index_path, int row_length);


/**
 * @brief creates an index from the list of packages
 * 
 * @param packages the array of packages to create an index from
 * @param num_packages the number of packages in the array
 * @param index_path the path to write the comet index
 * @param record_length the byte length of the serialized package
 */
void index_create(
    PackageManifest* pm,
    const char* index_path,
    int record_length
);


/**
 * @brief alters existing index with new package list
 * 
 * @param index the parsed packages from the original index
 * @param new_packages the new packages to alter index with
 * @param index_path the location of the comet index path
 * @param row_length the byte length of the serialized package
 */
// void index_alter(
//     Package** index,
//     Package** new_packages,
//     const char* index_path,
//     int row_length
// );


int index_meta(char** index_path, int* row_length);

/**
 * @brief delete the item from given index if it exists
 * 
 * @param index_path the path of the index to delete
 * @param row_length the byte length of the serialized package
 * @param offset the row offset of which row to delete
 * @return int 
 */
// int index_delete(const char* index_path, int row_length, int offset);


/**
 * @brief scan the index for requested id
 * 
 * @param p the package to search for
 * @param id the name to look for
 * @param index_path location of the comet index file
 * @param row_length the byte length of serialized package
 * @param start the start byte for the targeted attribute
 * @param end the end byte for the targeted attribute
 * @param found flag reference if found
 * @return int return < 0 if error encountered
 */
// int index_scan(
//     Package* p,
//     const char* id,
//     const char* index_path,
//     int row_length,
//     int start,
//     int end,
//     int* found
// );

/**
 * @brief loads the index into an array of package objects
 * 
 * @param packages the packages array to create
 * @return the number of packages loaded
 */
int index_load(
    Package*** packages,
    int* row_length
);

int index_verify(Package*** packages, int packages_sz, const char* package_name);

int index_update(Package*** packages, int packages_sz, PackageManifest* manifest);

#endif