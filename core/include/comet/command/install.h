#ifndef COMET_COMMAND_INSTALL_H_ 
#define COMET_COMMAND

#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include <stdlib.h>
#include <string.h>

#include "comet/index.h"
#include "comet/package.h"
#include "comet/setup.h"
#include "comet/picojson.h"

/**
 * @brief 
 * 
 * @param packages 
 * @param comet_dir 
 * @param package 
 * @param index_path 
 * @param row_length 
 * @return int 
 */
int install(Package* packages,
            const char* comet_dir,
            char* package,
            const char* index_path,
            int row_length);

void install(const char* package_name);

bool installed(Package** packages, int packages_sz, const char* name);

struct PackageManifest;
int install_pipe(PackageManifest* pm);

#endif