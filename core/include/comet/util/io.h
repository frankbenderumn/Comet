/**
 * @file io.h
 * @author Frank Bender (fjbender2@gmail.com)
 * @brief Composite file of I/O manipulation functionality. Predominantly
 * C-based and written for POSIX-based systems. Future enhancements will be
 * focused on cross-platform implementation and optimization.
 * @version 0.1 Alpha
 * @date 2023-09-20
 * 
 * @copyright Copyright © 2023 Jericho, LLC. All Rights Reserved.
 * 
 */

#ifndef COMET_UTIL_IO_H_
#define COMET_UTIL_IO_H_

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>

#include "comet/util/color.h"

/**
 * @brief C++ function to read contents of file path into one std::string
 * object.
 * 
 * @param path 
 * @return std::string 
 */
std::string fread(const char* path);

/**
 * @brief Disables terminal output.
 *
 * @param original_stdout File descriptor of terminal I/O manager.
 */
void disable_terminal(int* original_stdout);


/**
 * @brief Enables terminal output
 *
 * @param original_stdout File descriptor of terminal I/O manager.
 */
void enable_terminal(int* original_stdout);


/**
 * @brief Checks if file exists
 *
 * @param filename the file to check
 * @return int returns < 0 if error encountered
 */
int exists(const char* filename);


/**
 * @brief Returns when file was last updated
 *
 * @param filename the file to check
 * @return time_t returns the unix timestamp of the update
 */
time_t updated_at(const char* filename);


/**
 * @brief Returns when file was last created
 *
 * @param filename the file to check
 * @return time_t returns the unix timestamp of the creation
 */
time_t created_at(const char* filename);


/**
 * @brief Create a dir object given the path
 *
 * @param path the path of the directory
 * @return int returns < 0 if error encountered
 */
int create_dir(const char *path);


/**
 * @brief remove directory specified in path
 *
 * @param path the directory to remove
 * @return int returns < 0 if error encountered
 */
int remove_dir(const char* path);


/**
 * @brief list files in directory and assign the number of files
 *
 * @param files reference variable to store files in directory
 * @param directory the directory to list files from
 * @param count the reference for the number of files
 * @return return < 0 if error encountered
 */
int list_files(char*** files, char* directory, int* count);


/**
 * @brief Read binary file from the given file
 *
 * @param filename the file to read
 * @param size a reference to assign the size of the file
 * @return unsigned* the byte string of the binary file
 */
unsigned char* read_bin(const char* filename, long* size);


/**
 * @brief Write binary file to the given file path
 *
 * @param filename the path of the file to write to
 * @param data the data to write
 * @param size the size of the data to write
 */
void write_bin(const char* filename, const void* data, size_t size);

/**
 * @brief enumerable for path types
 *
 */
enum PathType {
    PATH_FILE,
    PATH_DIR,
    PATH_UNKNOWN
};

/**
 * @brief Returns whether a path is a file or directory
 *
 * @param path
 * @return FileType
 */
PathType path_type(const char* path);

/**
 * @brief Returns the length of a char** array
 *
 * @param arr the array to get length of
 * @return int the length of the array
 */
int len(char** arr);

/**
 * @brief removes file with error checking. WARN: This function is probably
 * redundant
 *
 * @param path the path to remove
 * @return int returns < 0 if error encountered
 */
int fremove(const char* path);


/**
 * @brief Returns the size of the file pointer
 *
 * @param fp the file pointer to seek on
 * @return long the size of the file in bytes
 */
long fsize(FILE* fp);


/**
 * @brief returns remainder after last occurence of delimiter in string
 *
 * @param result the remainder after the delim
 * @param str the string to find char in
 * @param delim the delimiter to find the last of
 * @return int returns < 0 if error encountered
 */
int last_of(char* result, const char* str, char delim);


/**
 * @brief pads the byte string to a specified width
 *
 * @param bytes the byte string to pad
 * @param bytes_len the size of the byte string without padding
 * @param width the desired width of the string, will pad if needed
 * @return int returns < 0 if error encountered
 */
int pad(bytes_t bytes, size_t bytes_len, size_t width);


/**
 * @brief Frees the malloc'd directories created from the create_dirs function
 *
 * @param dirs the malloc'd dirs
 * @param sz the number of dirs in the array
 */
void free_dirs(char** dirs, int sz);


/**
 * @brief removes directory with any nesting
 *
 * @param path the directory to remove
 * @return int returns < 0 if error encountered
 */
int remove_dir_r(const char *path);


/**
 * @brief removes directory specified in path
 *
 * @param path the directory to remove
 * @return int returns < 0 if error encountered
 */
int remove_dir(const char* path);


/**
 * @brief loads directories of the package
 *
 * @param dirs WARN: unknown
 * @param comet_dir the directory where comet resides in
 * @param package_name the name of the package
 * @param file_ct WARN: unknown
 * @return char**
 */
char** load_dirs(
    char** dirs,
    char* comet_dir,
    const char* package_name,
    int* file_ct
);

#endif