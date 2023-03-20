#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#include <string>

#include "comet/comet.h"

void red(void) { printf("\033[0;31m"); }
void blue(void) { printf("\033[0;34m"); }
void yellow(void) { printf("\033[0;33m"); }
void green(void) { printf("\033[0;32m"); }
void cyan(void) { printf("\033[0;36m"); }
void purple(void) { printf("\033[0;35m"); }
void magenta(void) { printf("\033[0;35m"); }
void black(void) { printf("\033[0;30m"); }
void white(void) { printf("\033[0;37m"); }
void grey(void) { printf("\033[0;90m"); }
void lred(void) { printf("\033[0;91m"); }
void lgreen(void) { printf("\033[0;92m"); }
void lyellow(void) { printf("\033[0;93m"); }
void lblue(void) { printf("\033[0;94m"); }
void lpurple(void) { printf("\033[0;95m"); }
void lmagenta(void) { printf("\033[0;95m"); }
void lcyan(void) { printf("\033[0;96m"); }
void lwhite(void) { printf("\033[0;97m"); }

void redbg(void) { printf("\033[0;41m"); }
void bluebg(void) { printf("\033[0;44m"); }
void yellowbg(void) { printf("\033[0;43m"); }
void greenbg(void) { printf("\033[0;42m"); }
void cyanbg(void) { printf("\033[0;46m"); }
void purplebg(void) { printf("\033[0;45m"); }
void magentabg(void) { printf("\033[0;45m"); }
void blackbg(void) { printf("\033[0;40m"); }
void whitebg(void) { printf("\033[0;47m"); }
void greybg(void) { printf("\033[0;100m"); }
void lredbg(void) { printf("\033[0;101m"); }
void lgreenbg(void) { printf("\033[0;102m"); }
void lyellowbg(void) { printf("\033[0;103m"); }
void lbluebg(void) { printf("\033[0;104m"); }
void lpurplebg(void) { printf("\033[0;105m"); }
void lmagentabg(void) { printf("\033[0;105m"); }
void lcyanbg(void) { printf("\033[0;106m"); }
void lwhitebg(void) { printf("\033[0;107m"); }

void bold(void) { printf("\033[1m"); }
void underscore(void) { printf("\033[4m"); }
void blink(void) { printf("\033[5m"); }
void reverse(void) { printf("\033[7m"); }
void conceal(void) { printf("\033[8m"); }
void clearcolor(void) { printf("\033[0m"); }

void br(void) { printf("\n"); }

#define RED(...) red(); printf(__VA_ARGS__); clearcolor();
#define BLU(...) blue(); printf(__VA_ARGS__); clearcolor();
#define YEL(...) yellow(); printf(__VA_ARGS__); clearcolor();
#define MAG(...) magenta(); printf(__VA_ARGS__); clearcolor();
#define GRE(...) green(); printf(__VA_ARGS__); clearcolor();
#define CYA(...) cyan(); printf(__VA_ARGS__); clearcolor();
#define WHI(...) white(); printf(__VA_ARGS__); clearcolor();
#define GRY(...) grey(); printf(__VA_ARGS__); clearcolor();
#define BLA(...) black(); printf(__VA_ARGS__); clearcolor();

#define BRED(...) red(); bold(); printf(__VA_ARGS__); clearcolor();
#define BBLU(...) blue(); bold(); printf(__VA_ARGS__); clearcolor();
#define BYEL(...) yellow(); bold(); printf(__VA_ARGS__); clearcolor();
#define BMAG(...) magenta(); bold(); printf(__VA_ARGS__); clearcolor();
#define BGRE(...) green(); bold(); printf(__VA_ARGS__); clearcolor();
#define BCYA(...) cyan(); bold(); printf(__VA_ARGS__); clearcolor();
#define BWHI(...) white(); bold(); printf(__VA_ARGS__); clearcolor();
#define BGRY(...) grey(); bold(); printf(__VA_ARGS__); clearcolor();
#define BBLA(...) black(); bold(); printf(__VA_ARGS__); clearcolor();

typedef unsigned char* bytes_t;
typedef unsigned char BYTE;

int exists(const char* filename) {
    return access(filename, F_OK) != -1;
}

time_t updated_at(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("Error in stat");
        exit(EXIT_FAILURE);
    }
    return st.st_mtime;
}

time_t created_at(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("Error in stat");
        exit(EXIT_FAILURE);
    }
    return st.st_ctime;
}

int create_dir(const char *path) {
    struct stat st = {0};
    int result = 0;
    
    if (stat(path, &st) == -1) {
        result = mkdir(path, 0700);
        if (result == -1) {
            printf("Error: could not create directory\n");
            return -1;
        }
        printf("Directory created successfully\n");
    }
    return 0;
}

char** list_files(char* directory, int* count) {
    DIR *dir;
    struct dirent *ent;
    char** files = NULL;
    int i = 0;
    
    dir = opendir(directory);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                char* file_name = (char*)malloc(strlen(directory) + strlen(ent->d_name) + 1);
                strcpy(file_name, directory);
                strcat(file_name, ent->d_name);
                files = (char**)realloc(files, (i + 1) * sizeof(char*));
                files[i++] = file_name;
            }
        }
        closedir(dir);
    } else {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
    *count = i;
    return files;
}

unsigned char* read_bin(const char* filename, long* size) {
    FILE* fp;
    unsigned char* buffer;
    size_t result;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        *size = -1;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    rewind(fp);

    buffer = (unsigned char*)malloc(*size);
    if (buffer == NULL) {
        fclose(fp);
        *size = -1;
        return NULL;
    }

    result = fread(buffer, 1, *size, fp);
    if (result != *size) {
        free(buffer);
        fclose(fp);
        *size = -1;
        return NULL;
    }

    fclose(fp);
    return buffer;
}

void write_bin(const char* filename, const void* data, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file '%s' for writing\n", filename);
        exit(1);
    }
    fwrite(data, 1, size, file);
    fclose(file);
}

int len(char** arr) {
    int count = 0;
    while (arr[count] != NULL) {
        count++;
    }
    return count;
}

int pipe(const char* script, char*** args) {
    pid_t pid;
    int status;

    BGRE("Jesus Fucking Christ!\n");

    if (*args == NULL) {
        BRED("Amelia ghosted you!\n");
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
                BRED("Subprocess did not terminate normally!\n");
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

int remove_dir(const char* path) {
    DIR* dir = opendir(path);
    struct dirent* entry;

    if (!dir) {
        BRED("Error opening directory '%s'\n", path);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char* filepath = (char*)malloc(strlen(path) + strlen(entry->d_name) + 2);
        sprintf(filepath, "%s/%s", path, entry->d_name);

        if (remove(filepath) == -1) {
            BRED("Error removing file '%s'\n", filepath);
            free(filepath);
            continue;
        }

        free(filepath);
    }

    if (rmdir(path) == -1) {
        BRED("Error removing directory '%s'\n", path);
        return - 1;
    }

    closedir(dir);
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

int main(int argc, char* argv[]) {
    BYEL("Running comet...\n");

    if (argc < 2) {
        printf("\033[1;31mComet: Invalid number of arguments!\033[0m\n");
    }

    char* command = argv[1];
    char* opt = NULL; 

    if (argc > 2) {
        opt = argv[2];
    }

    int row_length = 136;
    const char* index_path = "./.internal/index";
    Package* packages = NULL;

    if (comet_setup() < 0) {
        BRED("Corrupted file structure on comet!\n");
        return -1;
    }

    if (strcmp(command, "init") != 0) {
        if (verify_dirs() < 0) {
            BRED("Comet has not been initialized, please run 'sudo comet init'\n");
            return -1;
        }
    }

    if (strcmp(command, "install") == 0) {

        MAG("Installing...\n");
        std::string s = "./bin/" + std::string(opt);
        MAG("command: %s\n", s.c_str());

        int num_packs = 0;
        if (load_index(&packages, &num_packs, index_path, row_length) < 0) {
            BRED("Failed to load index!\n");
            return -1;
        }

        char script_dir[] = "./.internal/package/";
        
        int num_scripts = 0;
        char** scripts = list_files((char*)script_dir, &num_scripts);
        char** names = (char**)malloc(sizeof(char*) * num_scripts);
        int i = 0;
        BCYA("Available scripts:\n=====================================\n");
        while (i < num_scripts) {
            CYA("Script: %s\n", scripts[i]);
            char buf[32];
            if (last_of(buf, scripts[i], '/') < 0) {
                BRED("Failed to find last of '/'");
                return -1;
            }
            BCYA("Id: %s\n", buf);
            char* aname = (char*)malloc(sizeof(char) * 32);
            memcpy(aname, buf, 32);
            aname[32] = 0;
            scripts[i] = aname;
            i++;
        }

        BYEL("Iterate script names\n======================================\n");
        i = 0;
        int match = 0;
        while (i < num_scripts) {
            YEL("Name: %s\n", scripts[i]);
            if (strcmp(scripts[i], opt) == 0) {
                match = 1;
            }
            i++;
        }

        // remove for uninstall debug
        if (!match) {
            BRED("No package found for name '%s'\n", opt);
            return -1;
        }

        if(exists(index_path)) {

            Package* p = (Package*)malloc(sizeof(Package));
            int found = 0;
            if (scan_index(p, opt, index_path, row_length, 0, 16, &found) < 0) {
                BRED("Failed to scan index for: %s\n", scripts[i]);
                return 0;
            }

            if (!found) {
                RED("Package '%s' not found in index! Installing...\n", opt);
            } else {
                BGRE("Package '%s' already installed!\n", p->name);
                return 0;
            }

        }

//////////////////////////////////////////////////
        const char* package_dir = "./.internal/package/";
        char* package_path = (char*)malloc(sizeof(char) * (strlen(package_dir) + strlen(opt) + 1));

        strncpy(package_path, package_dir, strlen(package_dir));
        strncpy(package_path + strlen(package_dir), opt, strlen(opt));
        package_path[strlen(package_dir) + strlen(opt)] = 0;

        BMAG("Package path: %s\n", package_path);

        long sz = 0;
        bytes_t script_bin = read_bin(package_path, &sz);
        char script_ascii[sz - 64 + 1];
        memcpy(script_ascii, script_bin + 64, sz);
        script_ascii[sz - 64] = 0;

        BMAG("Ascii script\n");
        MAG("%s\n", (char*)script_ascii);

        const char* exec_dir = "./.internal/exec/";

        if (create_dir(exec_dir) < 0) {
            BRED("Failed to create exec dir for command script!\n");
            free(script_bin);
            free(package_path);
            return -1;
        }

        char* exec_path = (char*)malloc(sizeof(char) * (strlen(exec_dir) + strlen(opt) + 1));
        char* temp_path = (char*)malloc(sizeof(char) * (strlen(exec_dir) + 4 + 1));
        char* exec_file = (char*)malloc(sizeof(char) * (sz - 64));

        memcpy(exec_path, exec_dir, strlen(exec_dir));
        memcpy(exec_path + strlen(exec_dir), opt, strlen(opt));
        // memcpy(exec_path + strlen(exec_dir) + strlen(package_name), ".sh", 3);
        exec_path[strlen(exec_dir) + strlen(opt) + 3] = 0;

        memcpy(temp_path, exec_dir, strlen(exec_dir));
        memcpy(temp_path + strlen(exec_dir), "temp", 4);
        temp_path[strlen(exec_dir) + 4] = 0;


        BBLU("Exec path is: %s\n", exec_path);
        BBLU("Temp path is: %s\n", temp_path);

        FILE* to_exec = fopen(temp_path, "wb");

        if (!to_exec) {
            BRED("Failed to open for write executable for path '%s'\n", exec_path);
            return -1;
        }

        fwrite(script_ascii, 1, sz - 64, to_exec);
        fclose(to_exec);

        FILE* input = fopen(temp_path, "rb");
        if (!input) {
            fprintf(stderr, "Error: failed to open input file %s\n", argv[1]);
            return 1;
        }

        FILE* output = fopen(exec_path, "wb");
        if (!output) {
            fprintf(stderr, "Error: failed to open output file %s\n", argv[2]);
            fclose(input);
            return 1;
        }

        int ch;
        int last_ch = '\n';
        while ((ch = fgetc(input)) != EOF) {
            if (ch == '\r') {
                // Replace CRLF with LF
                if (last_ch == '\n') {
                    fseek(output, -1, SEEK_CUR);
                }
                ch = '\n';
            }
            fputc(ch, output);
            last_ch = ch;
        }

        fclose(input);
        fclose(output);

        // this malloc with null element works.... \n
        // need to cleanup mem appropriately here
        char** passables = (char**)malloc(sizeof(char*) * 4);
        passables[0] = strdup("sh");
        passables[1] = strdup("install");
        passables[2] = strdup("placeholder");
        passables[3] = 0;

        if (chmod(exec_path, 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        char syscom[10 + strlen(exec_path)];
        memcpy(syscom, "dos2unix ", 9);
        memcpy(syscom + 9, exec_path, strlen(exec_path));
        syscom[9 + strlen(exec_path)] = 0;

        BYEL("Syscom: %s\n", syscom);

        // system(syscom);

        int status;
        if ((status = pipe(exec_path, &passables)) < 0) {
            BRED("Failed to pipe command '%s'\n", exec_path);
            exit(EXIT_FAILURE);
            status = -1;
            return -1;
        }

        BBLU("Status is: %i\n", status);

        if (status != 0) {
            BRED("Failure\n");
            return -1;
        }

        BRED("Need to delete exec dir after! For security reasons.\n");

        free(passables[0]);
        free(passables[1]);
        free(passables[2]);
        free(passables);

        free(temp_path);
        free(exec_path);
        free(exec_file);
        free(script_bin);
        free(package_path);

        if (remove_dir(exec_dir) < 0) {
            BRED("Failed to remove directory!\n");
            return -1;
        }

///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

        BRED("Num packages in index: %i\n", num_packs);
        BYEL("====================================\n");
        BYEL("               Index                \n");
        BYEL("====================================\n");
        dump_index(packages);

        Package* packages_new = NULL;
        char script[32];
        printf("Opt is: %s\n", opt);
        strncpy(script, "./bin/", 6);
        strncpy(script + 6, opt, strlen(opt));
        script[6 + strlen(opt)] = 0;
        printf("Script is: %s\n", script);
        Package* to_install = (Package*)malloc(sizeof(Package));
        MemMap* mmap3 = (MemMap*)malloc(sizeof(MemMap) * 6);
        package_create(to_install, &packages_new, opt, 1, script, &mmap3, 6);
        free(mmap3);

        BYEL("====================================\n");
        BYEL("              To Add                \n");
        BYEL("====================================\n");
        dump_index(packages_new);

        BCYA("###############################\n");
        CYA("Index alteration!\n");

        alter_index(&packages, &packages_new, index_path, row_length);

        // BBLU("Program status: %i\n", status);

    cleanup:
        Package* head = packages;
        while (head != NULL) {
            Package* temp = head;
            head = head->next;
            free(temp);
        }

        head = packages_new;
        while (head != NULL) {
            Package* temp = head;
            head = head->next;
            free(temp);
        }
    } else if (strcmp(command, "help") == 0 || strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
        printf("======================================================================================================\n");
        printf("                                       Comet Package Manager                                          \n");
        printf("======================================================================================================\n");
        printf("%-32s: %s\n", "install", "Install a package within the registry");   
        printf("%-32s: %s\n", "uninstall", "Uninstall a package within the registry");   
        printf("%-32s: %s\n", "list", "List all packages with detailed information");   
        printf("%-32s: %s\n", "upload <script> <user>", "Upload a package to the registry with API token");   
        printf("%-32s: %s\n", "show <package_name>", "Get detailed information about a package");
        printf("%-32s: %s\n", "update <package_name> <new_script>", "Update package with a new script");
        printf("%-32s: %s\n", "script <package_name>", "For debug purposes, get the ascii bash script");
        printf("%-32s: %s\n", "load", "Reads package names from a comet.txt file to install required packages");

    } else if (strcmp(command, "uninstall") == 0) {
        BYEL("Uninstalling package...\n");
        Package* p = (Package*)malloc(sizeof(Package));
        int found = 0;
        if (scan_index(p, opt, index_path, row_length, 0, 16, &found) < 0) {
            BRED("Failed to scan index!\n");
            return -1;
        }

        if (!found) {
            printf("Package '%s' not installed!\n", opt);
            return 0;
        }

        if (delete_index(index_path, row_length, p->rowid) < 0) {
            BRED("Error removing package from index!\n");
            return -1;
        }

    } else if (strcmp(command, "show") == 0) {
        if (argc < 3) {
            BRED("Invalid number arguments for command show!\n");
            return -1;
        }
        int num_packages;
        if (load_index(&packages, &num_packages, index_path, row_length) < 0) {
            BRED("Failed to load index on command show!\n");
            return -1;
        }
        Package* head = packages;
        while (head != NULL) {
            if (strcmp(head->name, argv[2]) == 0) {
                show_package(head);
                break;
            }
            head = head->next;
        }

    } else if (strcmp(command, "list") == 0) {
        int num_packages;
        if (load_index(&packages, &num_packages, index_path, row_length) < 0) {
            BRED("Failed to load index on command show!\n");
            return -1;
        }
        Package* head = packages;
        while (head != NULL) {
            printf("===================================================\n");
            show_package(head);
            head = head->next;
        }
    } else if (strcmp(command, "upload") == 0) {
        if (argc != 4) {
            BRED("Invalid number of arguments for command upload!\n");
            return -1;
        }

        char* script = argv[2];
        char* signature = argv[3];
        const char* package_dir = "./.internal/package/";
        char* package_ptr = NULL;

        printf("Script path: %s\n", script);
        printf("Signature: %s\n", signature);

        package_ptr = strrchr(script, '/');

        if (package_ptr == NULL) {
            BRED("Package name contains no '/'\n");
            return -1;
        }

        char package_name[32];
        strncpy(package_name, package_ptr + 1, 32);

        printf("Package name: %s\n", package_name);

        if (!exists(script)) {
            BRED("Invalid script provided for upload!\n");
            return -1;
        }

        long sz;
        bytes_t script_bin = read_bin(script, &sz);
        bytes_t write_bin = (bytes_t)malloc(sizeof(BYTE) * (sz + 64));
        BYTE sig[] = {"howdsdfsdfsdfsdfsdfsdf"};
        if (pad(sig, 64) < 0) {
            BRED("Failed to pad signature!\n");
            free(write_bin);
            free(script_bin);
            return -1;
        }

        memcpy(write_bin, sig, 64);
        memcpy(write_bin + 64, script_bin, sz);

        char* package_path = (char*)malloc(sizeof(char) * (strlen(package_dir) + strlen(package_name) + 1));

        strncpy(package_path, package_dir, strlen(package_dir));
        strncpy(package_path + strlen(package_dir), package_name, strlen(package_name));
        package_path[strlen(package_dir) + strlen(package_name)] = 0;

        printf("Package_path: %s\n", package_path);

        FILE* fp = fopen(package_path, "wb");

        if (!fp) {
            BRED("Failed to write to file: %s\n", package_path);
            free(write_bin);
            free(script_bin);
            free(package_path);
            fclose(fp);
            return -1;
        }

        fwrite(write_bin, 1, sz + 64, fp);

        fclose(fp);
        free(script_bin);
        free(write_bin);
    } else if (strcmp(command, "script") == 0) {
        if (argc != 4) {
            BRED("Invalid number of arguments provided for command script!\n");
            return -1;
        }

        char* package_name = argv[2];
        char* script_command = argv[3];

        char script_dir[] = "./.internal/package/";

        char* package_path = (char*)malloc(strlen(package_name) + strlen(script_dir) + 1);

        strncpy(package_path, script_dir, strlen(script_dir));
        strncpy(package_path + strlen(script_dir), package_name, strlen(package_name));
        package_path[strlen(script_dir) + strlen(package_name)] = 0;

        printf("Package path: %s\n", package_path);

        if (!exists(package_path)) {
            BRED("Package does not exist in script call: %s\n", package_path);
            free(package_path);
            return -1;
        }

        int num_scripts = 0;
        char** scripts = list_files(script_dir, &num_scripts);
        char** names = (char**)malloc(sizeof(char*) * num_scripts);
        int i = 0;
        BCYA("Available scripts:\n=====================================\n");
        while (i < num_scripts) {
            CYA("Script: %s\n", scripts[i]);
            char buf[32];
            if (last_of(buf, scripts[i], '/') < 0) {
                BRED("Failed to find last of '/'");
                return -1;
            }
            BCYA("Id: %s\n", buf);
            char* aname = (char*)malloc(sizeof(char) * 32);
            memcpy(aname, buf, 32);
            aname[32] = 0;
            scripts[i] = aname;
            i++;
        }

        BYEL("Iterate script names\n======================================\n");
        i = 0;
        while (i < num_scripts) {
            YEL("Name: %s\n", scripts[i]);
            Package* p = (Package*)malloc(sizeof(Package));
            int found = 0;
            if (scan_index(p, scripts[i], index_path, row_length, 0, 16, &found) < 0) {
                BRED("Failed to scan index!\n");
                free(p);
                return -1;
            }
            BBLU("Need to append package to package linked list!\n");
            if (found) {
                BGRE("Successfully allocated package in mem!\n");
                printf("Package Flag: %i\n", p->flag);
            }
            i++;
        }

        long sz = 0;
        bytes_t script_bin = read_bin(package_path, &sz);
        char script_ascii[sz - 64 + 1];
        memcpy(script_ascii, script_bin + 64, sz);
        script_ascii[sz - 64] = 0;

        BMAG("Ascii script\n");
        MAG("%s\n", (char*)script_ascii);

        const char* exec_dir = "./.internal/exec/";

        if (create_dir(exec_dir) < 0) {
            BRED("Failed to create exec dir for command script!\n");
            free(script_bin);
            free(package_path);
            return -1;
        }

        char* exec_path = (char*)malloc(sizeof(char) * (strlen(exec_dir) + strlen(package_name) + 1));
        char* temp_path = (char*)malloc(sizeof(char) * (strlen(exec_dir) + 4 + 1));
        char* exec_file = (char*)malloc(sizeof(char) * (sz - 64));

        memcpy(exec_path, exec_dir, strlen(exec_dir));
        memcpy(exec_path + strlen(exec_dir), package_name, strlen(package_name));
        // memcpy(exec_path + strlen(exec_dir) + strlen(package_name), ".sh", 3);
        exec_path[strlen(exec_dir) + strlen(package_name) + 3] = 0;

        memcpy(temp_path, exec_dir, strlen(exec_dir));
        memcpy(temp_path + strlen(exec_dir), "temp", 4);
        temp_path[strlen(exec_dir) + 4] = 0;


        BBLU("Exec path is: %s\n", exec_path);

        FILE* to_exec = fopen(temp_path, "wb");

        if (!to_exec) {
            BRED("Failed to open for write executable for path '%s'\n", exec_path);
            return -1;
        }

        fwrite(script_ascii, 1, sz - 64, to_exec);
        fclose(to_exec);

        FILE* input = fopen(temp_path, "rb");
        if (!input) {
            fprintf(stderr, "Error: failed to open input file %s\n", argv[1]);
            return 1;
        }

        FILE* output = fopen(exec_path, "wb");
        if (!output) {
            fprintf(stderr, "Error: failed to open output file %s\n", argv[2]);
            fclose(input);
            return 1;
        }

        int ch;
        int last_ch = '\n';
        while ((ch = fgetc(input)) != EOF) {
            if (ch == '\r') {
                // Replace CRLF with LF
                if (last_ch == '\n') {
                    fseek(output, -1, SEEK_CUR);
                }
                ch = '\n';
            }
            fputc(ch, output);
            last_ch = ch;
        }

        fclose(input);
        fclose(output);

        // this malloc with null element works.... \n
        // need to cleanup mem appropriately here
        char** passables = (char**)malloc(sizeof(char*) * 4);
        passables[0] = strdup("sh");
        passables[1] = strdup(script_command);
        passables[2] = strdup("placeholder");
        passables[3] = 0;

        if (chmod(exec_path, 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        char syscom[10 + strlen(exec_path)];
        memcpy(syscom, "dos2unix ", 9);
        memcpy(syscom + 9, exec_path, strlen(exec_path));
        syscom[9 + strlen(exec_path)] = 0;

        BYEL("Syscom: %s\n", syscom);

        // system(syscom);

        int status;
        if ((status = pipe(exec_path, &passables)) < 0) {
            BRED("Failed to pipe command '%s'\n", exec_path);
            return -1;
        }

        BRED("Need to delete exec dir after! For security reasons.\n");

        free(passables[0]);
        free(passables[1]);
        free(passables[2]);
        free(passables);

        free(temp_path);
        free(exec_path);
        free(exec_file);
        free(script_bin);
        free(package_path);

    } else if (strcmp(command, "verify") == 0) {
        BMAG("Verifying package signatures...\n");

    } else if (strcmp(command, "register") == 0) {
        if (argc != 4) {
            BRED("Invalid number of arguments provided for register!\n");
            return -1;
        }
        char* package = argv[2];
        char* user = argv[3];

    } else if (strcmp(command, "newuser") == 0) {
        BYEL("Creating new user...\n");
        if (argc != 3) {
            BRED("Invalid number of arguments provided for newuser!\n");
            return -1;
        }

        char* newuser = argv[2];
    } else if (strcmp(command, "update_test") == 0) {
        BYEL("Update package test...\n"); 
        int num_packages = 0;
        if (load_index(&packages, &num_packages, index_path, row_length) < 0) {
            BRED("Failed to load index!\n");
            return -1;
        }
        Package* head = packages;
        while (head != NULL) {
            if (strcmp(head->name, opt) == 0) {
                BLU("Update time muthafucka!\n");
                head->version = 102;
                head->updated_at = (long)time(NULL);
                head->created_at = (long)0;
                memcpy(head->script, "ghosted_again", 14);
                head->script[14] = 0;
                printf("Headace creation: %li\n", head->created_at);
                printf("Headace updation: %li\n", head->updated_at);
                if (update_index_package(head, index_path, row_length) < 0) {
                    BRED("Failed to update index by package!\n");
                    return -1;
                }
            }
            head = head->next;
        }
    } else if (strcmp(command, "init") == 0) {
        // verify installation in /usr/local/comet
        if (create_dir("/usr/local/comet") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/source") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/source", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/include") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/include", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/lib") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/lib", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/build") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/build", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/compress") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/compress", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/env") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/env", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }


    }

    BGRE("Gracefully terminated!\n");
    return 0;
}