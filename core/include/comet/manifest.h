#ifndef COMET_MANIFEST_H_
#define COMET_MANIFEST_H_

#include <fstream>
#include <sstream>

#include <ctype.h>
#include <limits.h>
#include <regex.h>
#include <ctype.h>

#include "comet/io.h"
#include "comet/color.h"
#include "comet/picojson.h"
#include "comet/defs.h"

int safe_atoi(const char* str, int* result);

int semver_construct(SemVer* sv, char* version);

int semver_compare(SemVer* a, SemVer* b);

void manifest_dump(PackageManifest* pm);

// may need to alter for version specification
int uri_env(char* uri, const char* name, const char* version);

void manifest_array_destruct(PackageManifest*** arr, int sz);

PackageManifest* manifest_get_by_name(PackageManifest** manifest,
                                        int num, 
                                        const char* name);

int manifest_load(PackageManifest*** manifest, const char* comet_manifest);

#endif