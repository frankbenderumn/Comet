#ifndef COMET_DEFS_H_
#define COMET_DEFS_H_

#include <time.h>

enum Source {
    SOURCE_GITHUB
};

typedef struct CometAttr {
    int size;
    char type[16];
} CometAttr;

typedef struct Schema {
    CometAttr* attrs;
    int num_attrs;
} Schema;

typedef enum CometType {
    COMET_NULL,
    COMET_STR,
    COMET_INT,
    COMET_TS
} CometType;

typedef struct MemMap {
    void* addr;
    CometType type;
    int size;
} MemMap;

typedef struct SemVer {
    int major;
    int minor;
    int patch;
    char string[64];
} SemVer;

typedef struct PackageManifest {
    char name[32];
    char source[32];
    char uri[128];
    char uri_interpolated[128];
    char repo[32];
    SemVer* latest;
    char** versions;
    char** deps;
    int versions_sz;
    int deps_sz;
} PackageManifest;

typedef struct Package {
    PackageManifest* manifest;
    char signature[32];
    int row_id;
    time_t created_at;
    time_t updated_at;
    SemVer* version;
    int size;
} Package;

#endif