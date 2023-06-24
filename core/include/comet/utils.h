#ifndef COMET_UTILS_H_
#define COMET_UTILS_H_

#include <stdio.h>
#include <sys/wait.h>

#include "comet/io.h"

/** path restriction needed. i.e. do not allow /usr or ../ */
int pipe(const char* script, char*** args) {
    pid_t pid;
    int status;

    if (*args == NULL) {
        BRED("Should not happen!\n");
        return -1;
    }

    char** shit = *args;
    int lando = (sizeof(shit)/sizeof(char*));
    printf("Here we go again!: %i\n", lando);

    // size_t len = sizeof(*args) / sizeof((*args)[0]);

    printf("Array length is: %i\n", len(*args));

    /* Get and print my own pid, then fork and check for errors */
    // printf("My PID is %d\n", getpid());
    if ( (pid = fork()) == -1 ) {
        perror("Can't fork");
        return -1;
    }
    if (pid == 0) {
        if ( execv(script, *args) ) {
            perror("Can't exec");
            return -1;
        }
    } else if (pid > 0) {
        CYA("Forked!\n");
        /* In the parent we must wait for the child
            to exit with waitpid(). Afterward, the
            child exit status is written to 'status' */
        waitpid(pid, &status, 0);
        // printf("Child executed with PID %d\n", pid);
        // printf("Its return status was %d\n", status);
        // printf("Its return status was %d\n", status / 256);
        if (WIFEXITED(status)) {
            // may need to use signals for IPC :(
            if (status / 256 == 0) {
                // BGRE("Subprocess terminated normally!\n");
            } else if (status == 11) {
                BRED("Segfault detected in subprocess!\n");
                // exit(EXIT_FAILURE);
            } else {
                BRED("Subprocess did not terminate normally! Exited with code: %i\n", status);
                // exit(EXIT_FAILURE);
            }
        }

    } else {
        fprintf(stderr, "Something went wrong forking\n");
        return -1;
    }
    return status;
}

int pad(bytes_t bytes, int width) {
    // a demo
    // unsigned char str[] = { 'a', 'b', 'c' };
    // int width = 10;
    unsigned char *padded_str = (unsigned char *) malloc(width * sizeof(unsigned char));
    
    memcpy(padded_str, bytes, sizeof(bytes));
    memset(padded_str + sizeof(bytes), 0x00, width - sizeof(bytes));
    
    printf("Original string: ");
    for (int i = 0; i < sizeof(bytes); i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
    
    printf("Padded string:   ");
    for (int i = 0; i < width; i++) {
        printf("%02x ", padded_str[i]);
    }
    printf("\n");
    
    free(padded_str);
    return 0;
}

typedef struct Package {
    char name[17];
    long created_at; 
    long updated_at;
    char signature[65];
    int version;
    char script[33];
    Package* next;
    int id;
    int rowid;
    int flag;
} Package;

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

int package_serialize(Package* package, bytes_t data, size_t* size) {
    MemMap attrs[] = {
        {package->name, COMET_STR, 16},
        {&package->created_at, COMET_TS, 8},
        {&package->updated_at, COMET_TS, 8},
        {&package->version, COMET_INT, 4},
        {package->script, COMET_STR, 32},
        {package->signature, COMET_STR, 64},
        {&package->flag, COMET_INT, 4}
    };

    int lando = (sizeof(attrs)/sizeof(MemMap));

    int i = 0;
    int offset = 0;
    while (i < lando) {
        memcpy(data + offset, attrs[i].addr, attrs[i].size);
        offset += attrs[i].size;
        i++;
    }

    BBLU("Is the bug here?: %li\n", package->created_at);
    BBLU("Is the bug here?: %li\n", package->updated_at);

    *size = offset;
    return 0;
}

int package_create(Package* package, Package** packages, const char* name, int version, const char* script, MemMap** mmap, int num_attrs) {
    const char* package_dir = "./.internal/package/";
    char* package_path = (char*)malloc(sizeof(char) * (strlen(package_dir) + strlen(name) + 1));
    strncpy(package_path, package_dir, strlen(package_dir));
    strncpy(package_path + strlen(package_dir), name, strlen(name));
    package_path[strlen(package_dir) + strlen(name)] = 0;

    printf("Package name: %s\n", package_path);
    BMAG("MemMap Size is: %li\n", sizeof(mmap)/sizeof(mmap[0]));

    // default values
    strncpy(package->name, name, 16);
    package->name[16] = 0;
    package->version = version;
    package->created_at = time(NULL);
    package->updated_at = time(NULL);
    strncpy(package->script, script, 32);
    package->script[32] = 0;
    strncpy(package->signature, "123ds798aweiu123123", 64);
    package->signature[64] = 0;
    package->flag = 0;
    package->rowid = -1;
    // FILE* pack = NULL;
    // pack = fopen(package_path, "wb");
    // if (!pack) {
    //     BRED("Failed to write package to file!\n");
    //     free(package_path);
    //     fclose(pack);
    //     return -1;
    // }
    
    bytes_t data = (bytes_t)malloc(sizeof(unsigned char) * (16 + 8 + 8 + 4 + 32 + 64 + 4));
    size_t sz = 0;
    if (package_serialize(package, data, &sz) < 0) {
        BRED("Failed to serialize package!\n");
        free(package_path);
        free(data);
        // fclose(pack);
        return -1;
    }

    package->next = NULL;
    if (*packages == NULL) {
        *packages = package;
    } else {
        Package* head = *packages;
        while (head->next != NULL) {
            head = head->next;
        }
        head->next = package;
    }

    // fwrite(data, 1, sz, pack);
    free(package_path);
    free(data);
    // fclose(pack);
    return 0;
}

void dump_package(Package* package) {
    BBLU("-----------\n");
    printf("name: %s\n", package->name);
    printf("created_at: %li\n", package->created_at);
    printf("updated_at: %li\n", package->updated_at);
    printf("version: %i\n", package->version);
    printf("script: %s\n", package->script);
    printf("signature: %s\n", package->signature);
    printf("flag: %i\n", package->flag);
    printf("rowid: %i\n", package->rowid);
}

void show_package(Package* package) {
    struct tm* updated_tm = NULL;
    struct tm* created_tm = NULL;
    time_t created = (time_t)package->created_at;
    time_t updated = (time_t)package->updated_at;
    updated_tm = localtime((time_t*)&(package->created_at));
    created_tm = localtime((time_t*)&(package->updated_at));
    char created_str[32];
    char updated_str[32];
    strftime(created_str, sizeof(created_str), "%a, %b %d, %Y @ %I:%M:%S %p", created_tm);
    strftime(updated_str, sizeof(updated_str), "%a, %b %d, %Y @ %I:%M:%S %p", updated_tm);
    printf("%-16s: %s\n", "name", package->name);
    printf("%-16s: %s\n", "created at", created_str);
    printf("%-16s: %s\n", "updated at", updated_str);
    printf("%-16s: %i\n", "version", package->version);
    printf("%-16s: %s\n", "script", package->script);
    printf("%-16s: %s\n", "signature", package->signature);
    printf("%-16s: %i\n", "flag", package->flag);
    printf("%-16s: %i\n", "rowid", package->rowid);
}

void dump_index(Package* packages) {
    if (packages == NULL) {
        BRED("Package index is NULL!\n");
        return;
    }

    Package* head = packages;
    BMAG("Index dump\n");
    while (head != NULL) {
        dump_package(head);
        head = head->next;
    }
}

long fsize(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);
    return sz;
}

int add_index(Package** packages, int num_packages, const char* index_path, int record_length) {
    FILE* fp = fopen(index_path, "rb");
    
    int index_exist = 1;

    if (!fp) {
        BRED("Index does not exist!\n");
        index_exist = 0;
    }
    long sz = 0;
    bytes_t original = read_bin(index_path, &sz);
    if (fp) fclose(fp);

    Package* head = *packages;
    int byte_count = ((record_length * num_packages) + sz);
    bytes_t new_index = (bytes_t)malloc(sizeof(unsigned char) * byte_count);
    if (new_index == NULL) {
        char *err_str = strerror(errno);
        printf("Error: %s\n", err_str);
    }

    if (index_exist) { memcpy(new_index, original, sz); }

    int i = 0;
    while (head != NULL) {
        size_t record_sz;
        bytes_t data = (bytes_t)malloc(sizeof(BYTE) * record_length);
        if (package_serialize(head, data, &record_sz) < 0) {
            BRED("Failed to serialize package!\n");
            return -1;
        }
        memcpy(new_index + sz + (i * record_length), data, record_length);
        free(data);
        i++;
        head = head->next;
    }

    fp = fopen(index_path, "wb");
    
    if (!fp) {
        BRED("Failed to open file to write!\n");
        free(original);
        free(new_index);
        fclose(fp);
        return -1;
    }

    fwrite(new_index, 1, byte_count, fp);
    fclose(fp);
    free(original);
    free(new_index);

    return 0;
}

void we_neva_leavin_da_hood(const char* filename) {
    // Open the file for reading
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Failed to open script file");
        return;
    }

    // Determine the size of the file
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    // Allocate a buffer to hold the file contents
    char* buffer = (char*)malloc(sizeof(char) * size);
    if (buffer == NULL) {
        perror("Failed to allocate memory for script");
        fclose(fp);
        return;
    }

    // Read the file contents into the buffer
    size_t read_size = fread(buffer, sizeof(char), size, fp);
    if (read_size != size) {
        perror("Failed to read entire script file");
        free(buffer);
        fclose(fp);
        return;
    }

    // Close the file
    fclose(fp);

    // Add a null terminator to the buffer
    buffer[size] = '\0';

    // Split the buffer into arguments for execv
    int num_args = 0;
    char* token = strtok(buffer, " \n");
    while (token != NULL) {
        num_args++;
        token = strtok(NULL, " \n");
    }

    char** args = (char**)malloc(sizeof(char*) * (num_args + 1));
    if (args == NULL) {
        perror("Failed to allocate memory for arguments");
        free(buffer);
        return;
    }

    token = strtok(buffer, " \n");
    int i = 0;
    while (token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    // Execute the script using execv
    execv(args[0], args);

    // If execv returns, it must have failed
    perror("Failed to execute script");
    free(args);
    free(buffer);
    return;
}

int verify_index(Package* new_package, Package* index_package, const char* index_path) {
    FILE* fp = fopen(index_path, "rb");
    if (!fp) {
        BRED("update_index: Failed to open index file!\n");
        return -1;
    }

    if (new_package->updated_at != index_package->updated_at) {
        BCYA("update_index: modified timestamps detected!\n");
    }

    if (new_package->created_at != index_package->created_at) {
        BCYA("update_index: created timestamps detected!\n");
    }

    fclose(fp);

    return 0;
}

int update_index_package(Package* p, const char* index_path, int row_length) {
    if (!exists(index_path)) {
        BRED("Index file does not exist!\n");
        return -1;
    }

    if (p->rowid == -1) {
        BRED("Package not loaded from index! Unverified!\n");
        return -1;
    }

    BMAG("Single again   : %li\n", p->created_at);
    BMAG("Yay depression : %li\n", p->updated_at);

    size_t package_sz = 0;
    bytes_t package_data = (bytes_t)malloc(sizeof(BYTE) * row_length);
    if (package_serialize(p, package_data, &package_sz) < 0) {
        BRED("Failed to serialize package!\n");
        free(package_data);
        return -1;
    }

    if (package_sz != row_length) {
        BRED("Something went wrong with package serialization. Sizes don't match!\n");
        free(package_data);
        return -1;
    }

    long sz = 0;
    bytes_t bytes = read_bin(index_path, &sz);
    bytes_t new_index = (bytes_t)malloc(sizeof(BYTE) * sz);

    if (p->rowid == 0) {
        memcpy(new_index, package_data, row_length);
        memcpy(new_index + row_length, bytes + row_length, sz - row_length);
    } else if (p->rowid == (sz / row_length) - 1) {
        memcpy(new_index, bytes, sz - row_length);
        memcpy(new_index + (sz - row_length), package_data, row_length);
    } else {
        memcpy(new_index, bytes, (p->rowid * row_length));
        memcpy(new_index + (p->rowid * row_length), package_data, row_length);
        size_t cursor = (p->rowid + 1) * row_length;
        memcpy(new_index + cursor, bytes + cursor, sz - cursor);
    }

    FILE* fp = fopen(index_path, "wb");
    if (!fp) {
        BRED("Failed to open index file for writing!\n");
        free(bytes);
        free(new_index);
        free(package_data);
        return -1;
    }

    fwrite(new_index, 1, sz, fp);

    fclose(fp);
    free(bytes);
    free(new_index);
    free(package_data);
    
    return 0;
}

int update_index(const char* index_path, int rowid, int start, int end, void* value) {

    return 0;
}

void create_index(Package** packages, int num_packages, const char* index_path, int record_length) {
    Package* head = *packages;
    int byte_count = ((record_length * num_packages));
    unsigned char* new_index = (unsigned char*)malloc(sizeof(unsigned char) * byte_count);
    if (new_index == NULL) {
        char *err_str = strerror(errno);
        printf("Error: %s\n", err_str);
    }
    int i = 0;
    while (head != NULL) {
        size_t record_sz;
        bytes_t data = (bytes_t)malloc(sizeof(BYTE) * record_length);
        if (package_serialize(head, data, &record_sz) < 0) {
            BRED("Failed to serialize package!\n");
            return;
        }
        memcpy(new_index + (i * record_length), data, record_length);
        free(data);
        i++;
        head = head->next;
    }

    FILE* fp = fopen(index_path, "wb");
    
    if (!fp) {
        BRED("Failed to open file to write!\n");
        free(new_index);
        fclose(fp);
        return;
    }

    fwrite(new_index, 1, byte_count, fp);
    fclose(fp);
    free(new_index);
}

void alter_index(Package** index, Package** new_packages, const char* index_path, int row_length) {
    Package* index_head = *index;
    Package* package_head = *new_packages;
    Package** ptr = NULL;
    int match = 0;
    int old_len = 0;
    while (package_head != NULL) {
        while (index_head != NULL) {
            BMAG("%s (%li) == %s (%li)\n", package_head->name, sizeof(package_head->name), index_head->name, sizeof(index_head->name));
            if (strcmp(index_head->name, package_head->name) == 0) {
                BGRE("INDEX MATCH detected!\n");
                if (verify_index(package_head, index_head, index_path) < 0) {
                    return;
                }
                match = 1;
            }
            index_head = index_head->next;
        }
        if (match == 0) {
            BYEL("No match found in index!\n");
            if (old_len == 0) {
                ptr = (Package**)malloc(sizeof(Package*) * 2);
                ptr[0] = package_head;
                ptr[1] = 0;
                old_len++;
            } else {
                Package** extend = (Package**)realloc(ptr, (old_len + 2) * sizeof(Package*));
                for (int i = 0; i < old_len; i++) {
                    extend[i] = ptr[i];
                }
                extend[old_len] = package_head;
                extend[old_len+1] = 0;
                ptr = extend;
                old_len++;
            }
        }
        package_head = package_head->next;
        index_head = *index;
    }
    if (match == 0) {
        BRED("No index match detected!\n");
    }
    printf("Length of toAdd is: %i\n", old_len);
    for (int i = 0; i < old_len; i++) {
        printf("Package: %s\n", ptr[i]->name);
        printf("Version: %i\n", ptr[i]->version);
    }

    if (old_len > 0) {
        if (exists(index_path)) {
            add_index(ptr, old_len, index_path, row_length);
        } else {
            create_index(ptr, old_len, index_path, row_length);
        }
    }

    free(ptr);
}

int delete_index(const char* index_path, int row_length, int offset) {
    if (!exists(index_path)) {
        BRED("Index not created!\n");
        return -1;
    }

    long sz = 0;
    bytes_t data = read_bin(index_path, &sz);

    BRED("Mag size of index is: %li\n", sz);
    BRED("Num records is: %li\n", sz / row_length);

    bytes_t new_index = (bytes_t)malloc(sizeof(BYTE) * (sz - row_length));

    if (offset == 0) {
        memcpy(new_index, data + row_length, sz - row_length);
    } else if (offset == ((sz / row_length) - 1)) {
        memcpy(new_index, data, sz - row_length);
    } else {
        // lets offset is 5, row length is 100, total bytes is 700 
        // read the first 500 bytes
        memcpy(new_index, data, offset * row_length);
        // read last 100 bytes
        memcpy(new_index + (offset * row_length), data + ((offset + 1) * row_length), sz - ((offset + 1) * row_length));
    }

    BMAG("Verify bytes count: %li\n", (offset * row_length) + (sz - ((offset + 1) * row_length)));
    BGRE("New size of index should be: %li\n", sz - row_length);
    BGRE("New num of records should be: %li\n", (sz - row_length) / row_length);

    FILE* to_write = fopen(index_path, "wb");

    if (!to_write) {
        BRED("Failed to open index file for writing!\n");
        free(new_index);
        free(data);
        return -1;
    }

    fwrite(new_index, 1, sz - row_length, to_write);

    fclose(to_write);

    free(new_index);
    free(data);

    return 0;
}

int scan_index(Package* p, const char* id, const char* index_path, int row_length, int start, int end, int* found) {
    BBLU("Scanning index!\n");

    long sz;
    bytes_t bytes = read_bin(index_path, &sz);
    // printf("Size of index: %li\n", sz);
    int num_records = (int)((sz / row_length));

    BLU("Num records: %i\n", num_records);

    FILE* fp = fopen(index_path, "rb");
    if (!fp) {
        BRED("Failed to open index file!\n");
        fclose(fp);
        return -1;
    }
    BBLU("Index path: %s\n", index_path);
    BBLU("Row length: %i\n", row_length);
    BBLU("End - start: %i\n", end - start);
    char buffer[row_length];
    int j = 0;
    while (fread(buffer, 1, row_length, fp) == row_length) {
        char name[(end - start) + 1];
        memcpy(name, buffer + start, (end - start));
        name[end-start] = 0;
        BYEL("Scanned name %s against %s\n", name, id);
        if (strcmp(name, id) == 0) {
            BGRE("Match found! Now convert to package!\n");
            
            // dry up to separate function?
            MemMap attrs[] = {
                {p->name, COMET_STR, 16},
                {&p->created_at, COMET_TS, 8},
                {&p->updated_at, COMET_TS, 8},
                {&p->version, COMET_INT, 4},
                {p->script, COMET_STR, 32},
                {p->signature, COMET_STR, 64},
                {&p->flag, COMET_INT, 4}
            };

            int lando = (sizeof(attrs)/sizeof(MemMap));

            // BGRE("Number of MemMap static attrs: %i\n", lando);

            int i = 0;
            int offset = 0;
            while (i < lando) {
                memcpy(attrs[i].addr, buffer + offset, attrs[i].size);
                if (attrs[i].type == COMET_STR) {
                    ((char*)attrs[i].addr)[attrs[i].size] = 0;
                }
                offset += attrs[i].size;
                i++;
            }

            p->id = -1;
            p->next = NULL;
            p->rowid = j;

            *found = 1;

        }
        j++;
    }
    BMAG("Done scanning!\n");
    return 0;
}

int load_index(Package** packages, int* len, const char* index_path, int row_length) {
    int exist = 1;

    if (!exists(index_path)) {
        BRED("Index does not exist!\n");
        exist = 0;
    }

    FILE* index = NULL;

    if (exist) {
        // BLU("Reading bin file!\n");
        long sz;
        bytes_t bytes = read_bin(index_path, &sz);
        // printf("Size of index: %li\n", sz);
        int num_records = (int)((sz / row_length));
        // printf("Num records: %i\n", num_records);
        int i = 0;
        
        FILE* fp = NULL;
        fp = fopen("./.internal/index", "rb");

        if (!fp) {
            BRED("Index does not exist!\n");
        }

        char buffer[row_length];

        int j = 0;
        while (fread(buffer, 1, row_length, fp) == row_length) {
            // printf("Load index: Read chunk: (%i - %i)\n", j * row_length, (j * row_length) + row_length);

            Package* p = (Package*)malloc(sizeof(Package));
            MemMap attrs[] = {
                {p->name, COMET_STR, 16},
                {&p->created_at, COMET_TS, 8},
                {&p->updated_at, COMET_TS, 8},
                {&p->version, COMET_INT, 4},
                {p->script, COMET_STR, 32},
                {p->signature, COMET_STR, 64},
                {&p->flag, COMET_INT, 4}
            };

            int lando = (sizeof(attrs)/sizeof(MemMap));

            // BGRE("Number of MemMap static attrs: %i\n", lando);

            int i = 0;
            int offset = 0;
            while (i < lando) {
                memcpy(attrs[i].addr, buffer + offset, attrs[i].size);
                if (attrs[i].type == COMET_STR) {
                    ((char*)attrs[i].addr)[attrs[i].size] = 0;
                }
                offset += attrs[i].size;
                i++;
            }

            p->id = -1;
            p->next = NULL;
            p->rowid = j;

            if (*packages == NULL) {
                *packages = p;
            } else {
                Package* head = *packages;
                while (head->next != NULL) {
                    head = head->next;
                }
                head->next = p;
            }

            (*len)++;
            // dump_package(p);
            j++;
        }

        free(bytes);
        fclose(fp);
    }
    return 1;
}

int comet_setup() {
    if (create_dir("./.internal") < 0) {
        BRED("Failed to create internal directory!\n");
        return -1;
    }

    if (create_dir("./.internal/cache") < 0) {
        BRED("Failed to create internal cache directory!\n");
        return -1;
    }

    if (create_dir("./.internal/package") < 0) {
        BRED("Failed to create internal package directory!\n");
        return -1;
    }
    return 0;
}

int last_of(char* result, const char* str, char delim) {
    const char* package_ptr = strrchr(str, delim);

    if (package_ptr == NULL) {
        BRED("Package name contains no '%c'\n", delim);
        return -1;
    }

    strncpy(result, package_ptr + 1, 32);
    return 0;
}

int verify_dirs() {
    if (!exists("/usr/local/comet")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/source")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/include")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/lib")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/build")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/compress")) {
        BRED("Error initing comet dirs\n"); return -1;
    }

    if (!exists("/usr/local/comet/env")) {
        BRED("Error initing comet dirs\n"); return -1;
    }


    return 0;
}

#endif