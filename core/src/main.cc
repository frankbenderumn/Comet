#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

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

#define ADDR(str, var) magenta(); bold(); printf("ADDR for (%s): %p\n", str, (void*)var); clearcolor(); 

typedef unsigned char* bytes_t;
typedef unsigned char BYTE;

int exists(const char* filename) {
    return access(filename, F_OK) != -1;
}

void disable_terminal(int* original_stdout) {
    *original_stdout = dup(STDOUT_FILENO);
    // Redirect stdout to /dev/null
    int null_fd = open("/dev/null", O_WRONLY);
    dup2(null_fd, STDOUT_FILENO);
}

void enable_terminal(int* original_stdout) {
    dup2(*original_stdout, STDOUT_FILENO);
    close(*original_stdout);
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

int list_files(char*** files, char* directory, int* count) {
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    
    dir = opendir(directory);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                *files = (char**)realloc(*files, (i + 1) * sizeof(char*));
                (*files)[i] = (char*)malloc(strlen(directory) + strlen(ent->d_name) + 1);
                memcpy((*files)[i], directory, strlen(directory));
                memcpy((*files)[i] + strlen(directory), ent->d_name, strlen(ent->d_name));
                (*files)[i][strlen(directory) + strlen(ent->d_name)] = 0;
                i++;
            }
        }
        closedir(dir);
    } else {
        BRED("Unable to open directory: %s\n", directory);
        return -1;
    }
    *count = i;
    return 0;
}

unsigned char* read_bin(const char* filename, size_t* size) {
    FILE* fp;
    unsigned char* buffer;
    size_t result;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    rewind(fp);

    buffer = (unsigned char*)malloc(*size);
    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }

    result = fread(buffer, 1, *size, fp);
    if (result != *size) {
        free(buffer);
        fclose(fp);
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
    BGRE("=================================\n");
    BGRE("              Piping             \n");
    BGRE("=================================\n");

    pid_t pid;
    int status;

    if (*args == NULL) {
        return -1;
    }

    char** shit = *args;
    int lando = (sizeof(shit)/sizeof(char*));
    printf("Here we go again!: %i\n", lando);

    // size_t len = sizeof(*args) / sizeof((*args)[0]);

    printf("Array length is: %i\n", len(*args));

    int null_fd = open("/dev/null", O_WRONLY);
    if (null_fd < 0) {
        perror("open");
        return -1;
    }

    int saved_stdout = dup(STDOUT_FILENO);
    if (saved_stdout < 0) {
        perror("dup");
        return -1;
    }

    if (dup2(null_fd, STDOUT_FILENO) < 0) {
        perror("dup2");
        return -1;
    }
    /* Get and print my own pid, then fork and check for errors */
    // printf("My PID is %d\n", getpid());
    if ( (pid = fork()) == -1 ) {
        perror("Can't fork");
        if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
            perror("dup2");
            return -1;
        }

        close(saved_stdout);
        close(null_fd);
        return -1;
    }
    if (pid == 0) {
        if ( execv(script, *args) ) {
            perror("Can't exec");
            if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
                perror("dup2");
                return -1;
            }

            close(saved_stdout);
            close(null_fd);
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
        if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
            perror("dup2");
            return -1;
        }

        close(saved_stdout);
        close(null_fd);
        return -1;
    }

    if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
        perror("dup2");
        return -1;
    }

    close(saved_stdout);
    close(null_fd);

    return status;
}

int pad(bytes_t bytes, size_t bytes_len, size_t width) {
    if (bytes_len >= width) {
        // No padding necessary
        return 0;
    }
    
    bytes_t padded_bytes = (bytes_t) calloc(width, sizeof(BYTE));
    if (padded_bytes == NULL) {
        // Failed to allocate memory
        return -1;
    }
    
    memcpy(padded_bytes, bytes, bytes_len);
    memset(padded_bytes + bytes_len, 0x00, width - bytes_len);
    
    memcpy(bytes, padded_bytes, width);
    free(padded_bytes);
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

    // BBLU("Is the timestamp bug here?: %li\n", package->created_at);
    // BBLU("Is the timestamp bug here?: %li\n", package->updated_at);

    *size = offset;
    return 0;
}

int package_create(Package* package, Package** packages, const char* name, int version, const char* script, MemMap** mmap, int num_attrs) {
    const char* package_dir = "/usr/local/comet/.internal/package/";
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

size_t fsize(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    size_t sz = ftell(fp);
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
    size_t sz = 0;
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

    size_t sz = 0;
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
            // BMAG("%s (%li) == %s (%li)\n", package_head->name, sizeof(package_head->name), index_head->name, sizeof(index_head->name));
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
        if (package_head == NULL) {
            break;
        }
        package_head = package_head->next;
        index_head = *index;
    }
    if (match == 0) {
        BRED("No index match detected!\n");
    }
    // printf("Length of toAdd is: %i\n", old_len);
    // for (int i = 0; i < old_len; i++) {
    //     char* name_copy = strdup(ptr[i]->name);
    //     printf("Package: %s\n", name_copy);
    //     free(name_copy);
    //     char* v_copy = strdup(ptr[i]->name);
    //     printf("Package: %s\n", v_copy);
    //     free(v_copy);
    // }

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

    size_t sz = 0;
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

    size_t sz;
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
    // BBLU("Index path: %s\n", index_path);
    // BBLU("Row length: %i\n", row_length);
    // BBLU("End - start: %i\n", end - start);
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
    fclose(fp);
    free(bytes);
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
        size_t sz;
        bytes_t bytes = read_bin(index_path, &sz);
        // printf("Size of index: %li\n", sz);
        int num_records = (int)((sz / row_length));
        // printf("Num records: %i\n", num_records);
        int i = 0;
        
        FILE* fp = NULL;
        fp = fopen("/usr/local/comet/.internal/index", "rb");

        if (!fp) {
            BRED("Cannot open file index\n");
            free(bytes);
            return -1;
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
    if (create_dir("/usr/local/comet/.internal") < 0) {
        BRED("Failed to create internal directory!\n");
        return -1;
    }

    if (create_dir("/usr/local/comet/.internal/cache") < 0) {
        BRED("Failed to create internal cache directory!\n");
        return -1;
    }

    if (create_dir("/usr/local/comet/.internal/package") < 0) {
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

int remove_dir_r(const char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    char *full_path;

    if (!dir) {
        BRED("Error opening directory '%s'\n", path);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        full_path = (char *) malloc(strlen(path) + strlen(entry->d_name) + 2);
        sprintf(full_path, "%s/%s", path, entry->d_name);
        if (entry->d_type == DT_DIR) {
            remove_dir_r(full_path);
        } else {
            if (remove(full_path) == -1) {
                BRED("Error removing file '%s'\n", full_path);
                return -1;
            }
        }
        free(full_path);
    }

    if (rmdir(path) == -1) {
        BRED("Error removing directory '%s'\n", path);
        return -1;
    }

    closedir(dir);
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

    // if (!exists("/usr/local/comet/env")) {
    //     BRED("Error initing comet dirs\n"); return -1;
    // }

    if (!exists("/usr/local/comet/.internal")) {
        BRED("Error initing comet dirs 'internal\n"); return -1;
    }

    if (!exists("/usr/local/comet/.internal/env")) {
        BRED("Error initing comet dirs 'internal/env\n"); return -1;
    }

    if (!exists("/usr/local/comet/.internal/lib")) {
        BRED("Error initing comet dirs 'internal/lib\n"); return -1;
    }
    
    if (!exists("/usr/local/comet/.internal/registry")) {
        BRED("Error initing comet dirs 'internal/registry\n"); return -1;
    }
    
    if (!exists("/usr/local/comet/.internal/cache")) {
        BRED("Error initing comet dirs 'internal/cache\n"); return -1;
    }


    return 0;
}

int verify_package(char** paths) {
    int len = sizeof(paths)/sizeof(char*);
    if (!exists(paths[10])) { BRED("Package not in registry\n"); } 
    return 0;
}

char** load_dirs(char** dirs, char* comet_dir, const char* package_name, int* file_ct) {
    *file_ct = 12;
    dirs = (char**)calloc((size_t)*file_ct, sizeof(char*));

    // important to understand the difference between a string literal and malloc'd string
    dirs[0] = (char*)malloc(strlen(comet_dir) + 1); // char is 1 byte so no need for 1 * strlen(dir)
    memcpy(dirs[0], comet_dir, strlen(comet_dir));
    dirs[0][strlen(comet_dir)] = 0;
    
    const char* extensions[] = {
        ".internal/index", 
        ".internal/registry/", 
        ".internal/lib/", 
        ".internal/registry/", 
        ".internal/lib/", 
        "source/", 
        "include/", 
        "lib/", 
        "build/", 
        "deps/", 
        "sub/"
    };

    const int num_extensions = sizeof(extensions) / sizeof(extensions[0]);
    
    for (int i = 1; i < num_extensions + 1; i++) {
        // int path_len = strlen(comet_dir) + strlen(extensions[i-1]) + strlen(package_name) + 1;
        // char* path = (char*)malloc(path_len);
        // if (i != 0 && i != 10) {
        // snprintf(path, path_len, "%s%s%s", comet_dir, extensions[i-1], package_name);
        // } else {
        // }
        
        char* path = NULL;
        if (i < 4) {
            path = (char*)malloc(strlen(comet_dir) + strlen(extensions[i-1]) + 1);
            memcpy(path, comet_dir, strlen(comet_dir));
            memcpy(path + strlen(comet_dir), extensions[i-1], strlen(extensions[i-1]));
            path[strlen(extensions[i-1]) + strlen(comet_dir)] = 0;
        } else {
            int path_len = strlen(comet_dir) + strlen(extensions[i-1]) + strlen(package_name) + 1;
            path = (char*)malloc(path_len);
            snprintf(path, path_len, "%s%s%s", comet_dir, extensions[i-1], package_name);
        }

        dirs[i] = path;
        // BYEL("file_path: %s\n", dirs[i]);
    }
    
    return dirs;
}

void free_dirs(char** dirs, int sz) {
    int i = 0;
    while (i < sz) {
        // printf("Freeing dirs[%d]: %p\n", i, (void*)dirs[i]);
        if (dirs[i]) free(dirs[i]);
        i++;
    }
    free(dirs);
}

int install_package(Package** packages, char** dirs, const char* package_name, int row_length);

int load_comet(Package** packages, char* comet_dir, char* index_path, int row_length) {
    int status = 0;
    int num_packages = 0;
    char** package_names = NULL;
    char* package = NULL;
    char* buffer = NULL;
    char** dirs = NULL;
    int i = 0;
    int cursor = 0;
    int last = 0;
    size_t sz = 0;

    FILE* fp = fopen("./comet.txt", "rb");
    if (!fp) {
        BRED("No comet.txt file found!\n");
        status = -1;
        goto cleanup_load;
    }

    sz = fsize(fp);
    buffer = (char*)malloc(sizeof(char) * sz);
    fread(buffer, 1, sz, fp);

    while (i < sz) {
        if (buffer[i] == '\n') {
            // package = (char*)malloc(sizeof(char) * (cursor + 1));
            package_names = (char**)realloc(package_names, sizeof(char*) * (num_packages+1));
            package_names[num_packages] = (char*)malloc(sizeof(char) * (cursor + 1));
            memcpy(package_names[num_packages], buffer + last, cursor);
            package_names[num_packages][cursor] = 0;
            GRE("Package: %s\n", package_names[num_packages]);
            last += cursor + 1;
            cursor = 0;
            num_packages++;
            i++;
            continue;
        }
        cursor++;
        i++;
    }
    package_names = (char**)realloc(package_names, sizeof(char*) * (num_packages+1));
    package_names[num_packages] = (char*)malloc(sizeof(char) * (cursor + 1));
    memcpy(package_names[num_packages], buffer + last, cursor);
    package_names[num_packages][cursor] = 0;
    GRE("Package: %s\n", package_names[num_packages]);
    BBLU("Number of packages is: %i\n", num_packages);
    num_packages++;

    i = 0;

    while (i < num_packages) {
        printf("And da package: %s\n", package_names[i]);

        if (package_names[i]) {
            int dir_ct = 0;
            dirs = load_dirs(dirs, comet_dir, package_names[i], &dir_ct);

            Package* pkg = (Package*)malloc(sizeof(Package));
            int found = 0;
            if (scan_index(pkg, package_names[i], index_path, row_length, 0, 16, &found) < 0) {
                BRED("Error scanning index!\n");
            }

            if (pkg) free(pkg);

            BBLU("FML!\n");

            if (!found) {
                if (install_package(packages, dirs, package_names[i], row_length) < 0) {
                    BRED("Error installing package!\n\n\n");
                }
            }

            free_dirs(dirs, dir_ct);  // free dirs before reallocating it

            BGRE("Finished Installing package %s!\n", package_names[i]);
            FILE* index_fp = fopen(index_path, "rb");
            if (!index_fp) { status = -1; goto cleanup_load; }
            BBLU("Num records: %li\n\n", fsize(index_fp) / row_length);
            fclose(index_fp);
        }
        i++;
    }

cleanup_load:
    if (fp) fclose(fp);
    if (buffer) free(buffer);
    if (package_names) {
        for (int i = 0; i < num_packages; i++) {
            if (package_names[i]) free(package_names[i]);
        }
        free(package_names);
    }
    // if (*packages) {
    //     Package* head = *packages;
    //     while (head != NULL) {
    //         Package* next = head->next;
    //         if (head) free(head);
    //         head = next;
    //     }
    // }
    BGRE("Finished Installing packages!\n\n");
    return status;
}

int write_package(Package** packages, char* package_name, char* index_path, int row_length, char* exec_dir, int num_packs, char** dirs) {
    int status = 0;
    int i; 
    char* lib_dir = dirs[3];
    Package* to_install = NULL;
    MemMap* mmap3 = NULL;
    char lib_read_buffer[32];
    Package* packages_new = NULL;
    Package* head = NULL;
    size_t lib_path_size = 64;
    char script[32];

    char package_lib_path[strlen(lib_dir) + strlen(package_name) + 1];
    memcpy(package_lib_path, lib_dir, strlen(lib_dir));
    memcpy(package_lib_path + strlen(lib_dir), package_name, strlen(package_name));
    package_lib_path[strlen(lib_dir) + strlen(package_name)] = 0;

    BGRE("Package name: %s\n", package_name);

    const char* lib_source_dir = dirs[9];
    char lib_source[strlen(lib_source_dir) + 5 + 1];
    memcpy(lib_source, lib_source_dir, strlen(lib_source_dir));
    memcpy(lib_source + strlen(lib_source_dir), "/lib/", 5);
    lib_source[strlen(lib_source_dir) + 5] = 0;

    // BGRE("Lib source dir: %s\n", lib_source);

    int has_libs = 1;
    if (!exists(lib_source)) {
        BRED("Lib sources not found. Potentially header only package\n");
        has_libs = 0;
    }

    if (has_libs) {

        int libct = 0;
        char** libs = NULL;
        if (list_files(&libs, (char*)lib_source, &libct) < 0) {
            BRED("Failed to list directory files!\n");
            return -1;
        }

        char** files = NULL;

        i = 0;
        int j = 0;
        int num_libs = 0;
        while(i < libct) {
            // printf("Libfile: %s\n", libs[i]);
            char* ptr = strstr(libs[i], ".so");
            if (ptr != NULL) {
                char* lib_name = (char*)malloc(sizeof(char) * lib_path_size);
                if (last_of(lib_name, libs[i], '/') < 0) {
                    BRED("Failed to find the last of us '%c'\n", '/');
                    free(lib_name);
                    continue;
                }
                BBLU("Library name: %s\n", lib_name);
                files = (char**)realloc(files, (j + 1) * sizeof(char*));
                files[j] = lib_name;
                j++;
            }
            ptr = strstr(libs[i], ".a");
            if (ptr != NULL) {
                char* lib_name = (char*)malloc(sizeof(char) * lib_path_size);
                if (last_of(lib_name, libs[i], '/') < 0) {
                    BRED("Failed to find the last of us '%c'\n", '/');
                    free(lib_name);
                    continue;
                }
                BBLU("Library name: %s\n", lib_name);
                files = (char**)realloc(files, (j + 1) * sizeof(char*));
                files[j] = lib_name;
                j++;
            }
            i++;
        }
        num_libs = j;
        BBLU("Number of libs!: %i\n", num_libs);

        // BMAG("Package lib path is: %s\n", package_lib_path);

        FILE* lib_write = fopen(package_lib_path, "wb");

        if (!lib_write) {
            BRED("Failed to open package library index for write!\n");
            return -1;
        }

        i = 0;
        while (i < num_libs) {
            bytes_t lib_bytes = (bytes_t)malloc(sizeof(BYTE) * lib_path_size);
            if (pad((bytes_t)files[i], strlen((char*)files[i]), lib_path_size) < 0) {
                BRED("Failed to pad library name!\n");
                free(lib_bytes);
                return -1;
            }
            memcpy(lib_bytes, files[i], lib_path_size);
            fwrite(lib_bytes, 1, lib_path_size, lib_write);
            free(files[i]);
            free(lib_bytes);
            i++;
        }
        free(files);

        i = 0;
        while (i < libct) {
            if (libs[i]) free(libs[i]);
            i++;
        }
        if (libs) free(libs);

        fclose(lib_write);

        // FILE* lib_read = fopen(package_lib_path, "rb");
        // if (!lib_read) {
        //     BRED("Failed to open package library index for write!\n");
        //     status = -1;
        //     goto cleanup;
        // }

        // while (fread(lib_read_buffer, 1, lib_path_size, lib_read) == lib_path_size) {
        //     char some_str[lib_path_size];
        //     memcpy(some_str, lib_read_buffer, lib_path_size);
        //     // BMAG("Read lib: %s\n", some_str);
        // }

        // fclose(lib_read);

    }

    if (remove_dir(exec_dir) < 0) {
        BRED("Failed to remove directory!\n");
        status = -1;
        goto cleanup;
    }

    BRED("Num packages in index: %i\n", num_packs);
    // BYEL("====================================\n");
    BYEL("Index\n");
    BYEL("====================================\n");
    // dump_index(*packages);
    printf("Opt is: %s\n", package_name);
    strncpy(script, "./bin/", 6);
    strncpy(script + 6, package_name, strlen(package_name));
    script[6 + strlen(package_name)] = 0;
    printf("Script is: %s\n", script);
    to_install = (Package*)malloc(sizeof(Package));
    mmap3 = (MemMap*)malloc(sizeof(MemMap) * 6);
    package_create(to_install, &packages_new, package_name, 1, script, &mmap3, 6);
    free(mmap3);

    // BYEL("====================================\n");
    BYEL("To add\n");
    BYEL("====================================\n");
    // dump_index(packages_new);

    BCYA("###############################\n");
    CYA("Index alteration!\n");

    alter_index(packages, &packages_new, index_path, row_length);

cleanup:
    head = packages_new;
    while (head != NULL) {
        Package* temp = head;
        head = head->next;
        if (temp) free(temp);
    }
    return status;
}

int uninstall_package(char** dirs, char* opt, char* index_path, int row_length) {
    char** lib_files = NULL;
    int status = 0;

    char comet_lib_dir[] = "/usr/local/comet/lib/";
    char build_lib_dir[] = "/usr/local/comet/.internal/lib/";
    char include_dir[] = "/usr/local/comet/include/";
    char source_dir[] = "/usr/local/comet/source/";

    char comet_libs[strlen(comet_lib_dir) + strlen(opt) + 1];
    memcpy(comet_libs, comet_lib_dir, strlen(comet_lib_dir));
    memcpy(comet_libs + strlen(comet_lib_dir), opt, strlen(opt));
    comet_libs[strlen(comet_lib_dir) + strlen(opt)] = 0;

    char package_libs[strlen(build_lib_dir) + strlen(opt) + 1];
    memcpy(package_libs, build_lib_dir, strlen(build_lib_dir));
    memcpy(package_libs + strlen(build_lib_dir), opt, strlen(opt));
    package_libs[strlen(build_lib_dir) + strlen(opt)] = 0;

    char package_include[strlen(include_dir) + strlen(opt) + 1];
    memcpy(package_include, include_dir, strlen(include_dir));
    memcpy(package_include + strlen(include_dir), opt, strlen(opt));
    package_include[strlen(include_dir) + strlen(opt)] = 0;

    char package_source[strlen(source_dir) + strlen(opt) + 1 + 7];
    memcpy(package_source, source_dir, strlen(source_dir));
    memcpy(package_source + strlen(source_dir), opt, strlen(opt));
    memcpy(package_source + strlen(source_dir) + strlen(opt), "-source", 7);
    package_source[strlen(source_dir) + strlen(opt) + 7] = 0;

    BMAG("Uninstall comet libs: %s\n", comet_lib_dir);
    BMAG("Uninstall package libs: %s\n", package_libs);
    BMAG("Uninstall package include: %s\n", package_include);
    BMAG("Uninstall package source: %s\n", package_source);
    BMAG("Uninstall package build: %s\n", package_libs);

    size_t lib_path_size = 64;
    int comet_lib_ct = 0;
    char** libs = NULL;
    if (list_files(&libs, comet_lib_dir, &comet_lib_ct) < 0) {
        BRED("Failed to list directory files!\n");
        return -1;
    }

    int has_libs = 1;
    FILE* fp = fopen(package_libs, "rb");
    if (!fp) {
        BRED("Failed to open package libs file!\n");
        has_libs = 0;
    }

    // create a dependency graph?
    // will need a systematic way to handle github modules,
    // but for now a dumb workaround
    // c4 a sub module of rapidyaml
    // can maybe get this information from git
    if (strcmp(opt, "rapidyaml") == 0) {
        char c4_path[] = "/usr/local/comet/include/c4";
        if (remove_dir_r(c4_path) < 0) {
            BRED("Error removing directory: %s\n", c4_path);
        }
    }

    if (exists(package_source)) {
        if (remove_dir_r(package_source) < 0) {
            BRED("Error removing directory: %s\n", package_source);
        }
    }

    if (exists(package_include)) {
        if (remove_dir_r(package_include) < 0) {
            BRED("Error removing directory: %s\n", package_include);
        }
    }

    // cache before deletion?
    // if (remove_dir_r(package_build) < 0) {
    //     BRED("Error removing directory: %s\n", package_build);
    // }

    if (has_libs) {
        BBLU("Por que?\n");
        size_t lib_sz = 0;
        bytes_t lib_index = read_bin(package_libs, &lib_sz);
        BBLU("Debug bin size!: %li\n", lib_sz);
        free(lib_index);
        char buffer[32];
        while (fread(buffer, 1, lib_path_size, fp) == lib_path_size) {
            int i = 0;
            printf("Buffer lib: %s... ", (char*)buffer);
            int found = 0;
            while (i < comet_lib_ct) {
                // printf("Comet lib: %s\n", libs[i]);
                char buf[32];
                if (last_of(buf, libs[i], '/') < 0) {
                    BRED("last of / not found\n");
                    return -1;
                }
                if (strcmp(buf, buffer) == 0) {
                    BGRE("Found\n");
                    found = 1;
                    if (remove(libs[i]) == -1) {
                        BRED("Error removing file '%s'\n", libs[i]);
                    }
                    break;
                }
                i++;
            }
            if (!found) {
                BRED("Not found!\n");
            }
        }
    }

    BYEL("Uninstalling package...\n");
    Package* p = (Package*)malloc(sizeof(Package));
    int found = 0;
    if (scan_index(p, opt, index_path, row_length, 0, 16, &found) < 0) {
        BRED("Failed to scan index!\n");
        return -1;
    }

    if (!found) {
        printf("Package '%s' not installed!\n", opt);
        status = 1;
        goto cleanup;
    }

    if (delete_index(index_path, row_length, p->rowid) < 0) {
        BRED("Error removing package from index!\n");
        status = -1;
        goto cleanup;
    }

cleanup:
    int i = 0;
    while (i < comet_lib_ct) {
        if (libs[i]) { free(libs[i]); }
        i++;
    }
    if (p) { free(p); }
    if (fp) { fclose(fp); }
    if (libs) { free(libs); }
    return status;
}

int install_package(Package** packages, char** dirs, const char* package_name, int row_length) {
    // Pro Tip always initialize your ptrs or you might be fucked.

    int status = 0;
    char* index_path = dirs[1];
    char* registry_dir = dirs[2];
    char* root_dir = dirs[0];
    char** scripts = NULL;
    int num_scripts = 0, pstatus = 0;
    int i = 0;
    int match = 0;
    size_t sz = 0;
    bytes_t script_bin = NULL;
    char* script_ascii = NULL;
    char* exec_path = NULL;
    char* temp_path = NULL;
    char* exec_file = NULL;
    FILE* to_exec, *input, *output;
    int ch, last_ch;
    char** passables = NULL;


    BRED("Root dir is: %s (%li) (%li)\n", root_dir, strlen(root_dir), sizeof(root_dir));
    char* exec_dir = (char*)malloc(sizeof(char) * (strlen(root_dir) + strlen(".internal/exec") + 1));
    char* package_path = (char*)malloc(sizeof(char) * (strlen(registry_dir) + strlen(package_name) + 1));

    if (list_files(&scripts, registry_dir, &num_scripts) < 0) {
        BRED("Failed to list files for registry: %s\n", registry_dir);
        return -1;
    }

    int pkg_clean = 0;
    int num_packs = 0;
    if (load_index(packages, &num_packs, index_path, row_length) < 0) {
        BRED("Failed to load index!\n");
        status = -1;
        goto cleanup;
    }

    memcpy(exec_dir, dirs[0], strlen(dirs[0]));
    memcpy(exec_dir + strlen(dirs[0]), ".internal/exec", strlen(".internal/exec"));
    exec_dir[strlen(dirs[0]) + strlen(".internal/exec")] = 0;
    
    // BCYA("Available scripts:\n=====================================\n");
    while (i < num_scripts) {
        // CYA("Script: %s\n", scripts[i]);
        char buf[32];
        if (last_of(buf, scripts[i], '/') < 0) {
            BRED("Failed to find last of '/'\n");
            status = -1;
            goto cleanup;
        }
        // CYA("Id: %s\n", buf);
        char* aname = (char*)malloc(sizeof(char) * (strlen(buf) + 1));
        memcpy(aname, buf, strlen(buf) + 1);
        free(scripts[i]);
        scripts[i] = (char*)malloc(sizeof(char) * (strlen(aname) + 1));
        memcpy(scripts[i], aname, strlen(aname) + 1);
        free(aname);
        i++;
    }

    BYEL("Iterate script names\n======================================\n");
    i = 0;
    while (i < num_scripts) {
        YEL("Name: %s\n", scripts[i]);
        if (strcmp(scripts[i], package_name) == 0) {
            match = 1;
        }
        i++;
    }

    // remove for uninstall debug
    if (!match) {
        BRED("No package found for name '%s'\n", package_name);
        status = -1;
        pkg_clean = 1;
        goto cleanup;
    }

////////////////////////////////////////////////////////////////////////////////

    if(exists(index_path)) {

        Package* p = (Package*)malloc(sizeof(Package));
        int found = 0;
        if (scan_index(p, package_name, index_path, row_length, 0, 16, &found) < 0) {
            BRED("Failed to scan index for: %s\n", scripts[i]);
            status = -1;
            if (p) free(p);
            goto cleanup;
        }

        if (!found) {
            RED("Package '%s' not found in index! Installing...\n", package_name);
            if (p) free(p);
        } else {
            BGRE("Package '%s' already installed!\n", p->name);
            if (p) free(p);
            status = -1;
            goto cleanup;
        }

    }

    strncpy(package_path, registry_dir, strlen(registry_dir));
    strncpy(package_path + strlen(registry_dir), package_name, strlen(package_name));
    package_path[strlen(registry_dir) + strlen(package_name)] = 0;

    BMAG("Package path: %s\n", package_path);

    script_bin = read_bin(package_path, &sz);
    if (sz < 64) {
        BRED("File too small\n");
        status = -1;
        goto cleanup;
    }
    script_ascii = (char*)malloc((sz - 64 + 1 > 0 ? sz - 64 + 1 : 1));
    memcpy(script_ascii, script_bin + 64, sz - 64);
    script_ascii[sz - 64] = '\0';

    // BMAG("Ascii script\n");
    // MAG("%s\n", (char*)script_ascii);

    // BBLU("Exec dir: %s\n", exec_dir);

    if (create_dir(exec_dir) < 0) {
        BRED("Failed to create exec dir for command script!\n");
        status = -1;
        goto cleanup;
    }

    exec_path = (char*)malloc(sizeof(char) * (strlen(exec_dir) + strlen(package_name) + 1));
    temp_path = (char*)malloc(sizeof(char) * (strlen(exec_dir) + 4 + 1));
    exec_file = (char*)malloc(sizeof(char) * (sz - 64));

    memcpy(exec_path, exec_dir, strlen(exec_dir));
    memcpy(exec_path + strlen(exec_dir), package_name, strlen(package_name));
    exec_path[strlen(exec_dir) + strlen(package_name)] = 0;

    memcpy(temp_path, exec_dir, strlen(exec_dir));
    memcpy(temp_path + strlen(exec_dir), "temp", 4);
    temp_path[strlen(exec_dir) + 4] = 0;


    // BBLU("Exec path is: %s\n", exec_path);
    // BBLU("Temp path is: %s\n", temp_path);

    to_exec = fopen(temp_path, "wb");

    if (!to_exec) {
        BRED("Failed to open for write executable for path '%s'\n", exec_path);
        status = -1;
        goto cleanup;
    }

    fwrite(script_ascii, 1, sz - 64, to_exec);
    fclose(to_exec);

    input = fopen(temp_path, "rb");
    if (!input) {
        fprintf(stderr, "Error: failed to open input file %s\n", temp_path);
        return 1;
    }

    output = fopen(exec_path, "wb");
    if (!output) {
        fprintf(stderr, "Error: failed to open output file %s\n", exec_path);
        fclose(input);
        return 1;
    }

    last_ch = '\n';
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

    // // this malloc with null element works.... \n
    // // need to cleanup mem appropriately here
    passables = (char**)malloc(sizeof(char*) * 4);
    passables[0] = strdup("sh");
    passables[1] = strdup("install");
    passables[2] = strdup("placeholder");
    passables[3] = 0;

    if (chmod(exec_path, 0755) == -1) {
        printf("Error setting executable bit on temporary file.\n");
        return 1;
    }

    int std_fd;
    disable_terminal(&std_fd);

    if ((pstatus = pipe(exec_path, &passables)) < 0) {
        BRED("Failed to pipe command '%s'\n", exec_path);
        exit(EXIT_FAILURE);
        status = -1;
        enable_terminal(&std_fd);
        goto cleanup;
    }

    enable_terminal(&std_fd);

    BGRE("Status is: %i\n", pstatus);

    if (pstatus != 0) {
        BRED("Pipe did not run successfully!\n");
        status = -1;
        goto cleanup;
    }

    if (write_package(packages, (char*)package_name, index_path, row_length, exec_dir, num_packs, dirs) < 0) {
        status = -1;
        goto cleanup;
    }

cleanup:
    if (exec_dir) free(exec_dir);

    Package* head = *packages;
    while (head != NULL) {
        Package* temp = head;
        head = head->next;
        if (temp) free(temp);
    }

    for (int i = 0; i < num_scripts; i++) {
        if (scripts[i]) free(scripts[i]);
    }
    if (scripts) { free(scripts); }

    if (passables) {
        if (passables[0]) free(passables[0]);
        if (passables[1]) free(passables[1]);
        if (passables[2]) free(passables[2]);
        free(passables);
    }

    if (temp_path) free(temp_path);
    if (exec_path) free(exec_path);
    if (exec_file) free(exec_file);

    if (script_ascii) free(script_ascii);
    if (package_path) free(package_path);
    if (script_bin) free(script_bin);


    return status;
}

char** project_index(int* name_ct, const char* index_path, int row_length, int start, int end) {
    FILE* fp = fopen(index_path, "rb");

    if (!fp) {
        BRED("Failed to open index file: %s\n", index_path);
        return NULL;
    }

    size_t sz = fsize(fp);

    int ct = 0;
    char** names = NULL;
    unsigned char buffer[row_length];
    while (fread(buffer, 1, row_length, fp) == row_length) {
        char name[end - start + 1];
        memcpy(name, buffer + start, end - start);
        name[end - start] = 0;
        BBLU("Name: %s\n", name);
        char** new_names = (char**)realloc(names, sizeof(char*) * (ct + 1));
        names = new_names;
        names[ct] = (char*)malloc(strlen(name) + 1);
        memcpy(names[ct], name, strlen(name) + 1);
        ct++;
    }

    if (fp) { fclose(fp); }
    *name_ct = ct;

    return names;
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

    const char* index_path = "/usr/local/comet/.internal/index";
    const char* exec_dir = "/usr/local/comet/.internal/exec/";
    const char* lib_dir = "/usr/local/comet/.internal/lib/";
    const char* package_dir = "/usr/local/comet/.internal/registry/";
    char script_dir[] = "/usr/local/comet/.internal/registry/";
    char comet_dir[] = "/usr/local/comet/";

    int row_length = 136;

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

        if (argc < 3) {
            BRED("Invalid number of arguments for install!\n");
            return -1;
        }

        int status = 0;
        int file_ct = 0;
        char** dirs = load_dirs(dirs, comet_dir, opt, &file_ct); 

        if (install_package(&packages, dirs, opt, row_length) < 0) {
            BRED("Failed to install package!\n");
            status = -1;
            free_dirs(dirs, file_ct);
            return -1;
        }

        free_dirs(dirs, file_ct);
        return status;

    } else if (strcmp(command, "help") == 0 || strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
        printf("======================================================================================================\n");
        printf("                                       Comet Package Manager                                          \n");
        printf("======================================================================================================\n");
        printf("%-32s: %s\n", "install", "Install a package within the registry");   
        printf("%-32s: %s\n", "uninstall", "Uninstall a package");   
        printf("%-32s: %s\n", "uninstall -a", "Uninstall all packages");   
        printf("%-32s: %s\n", "list", "List all packages with detailed information");   
        printf("%-32s: %s\n", "upload <script> <user>", "Upload a package to the registry for a user");   
        printf("%-32s: %s\n", "show <package_name>", "Get detailed information about a package");
        // printf("%-32s: %s\n", "update <package_name> <new_script>", "Update package with a new script");
        printf("%-32s: %s\n", "load", "Reads package names from a comet.txt file to install required packages");

    } else if (strcmp(command, "load") == 0) {
        if (load_comet(&packages, comet_dir, (char*)index_path, row_length) < 0) {
            BRED("Failed to load comet file!\n");
            return -1;
        }

        // Package* head = packages;
        // while (head != NULL) {
        //     BBLU("Da fuq!\n");
        //     Package* next = head->next;
        //     if (head) free(head);
        //     head = next;
        // }

    } else if (strcmp(command, "uninstall") == 0) {
        if (argc < 3) {
            BRED("Invalid number of arguments for install!\n");
            return -1;
        }

        int uninstall_all = 0;
        if (strcmp(argv[2], "-a") == 0) {
            uninstall_all = 1;
        } else if (strcmp(argv[2], "--all") == 0) {
            uninstall_all = 1;
        }

        if (!uninstall_all) {
            int file_ct = 0;
            char** dirs = load_dirs(dirs, comet_dir, opt, &file_ct); 

            if (uninstall_package(dirs, opt, (char*)index_path, row_length) < 0) {
                BRED("Failed to uninstall package!\n");
                free_dirs(dirs, file_ct);
                return -1;
            }

            free_dirs(dirs, file_ct);

        } else {
            char** names = NULL;
            int name_ct = 0;
            names = project_index(&name_ct, index_path, row_length, 0, 16);
            int i = 0;
            while (i < name_ct) {
                int file_ct = 0;
                char** dirs = load_dirs(dirs, comet_dir, names[i], &file_ct); 
                GRE("Name: %s\n", names[i]);
                if (uninstall_package(dirs, names[i], (char*)index_path, row_length) < 0) {
                    BRED("Failed to uninstall package!\n");
                }
                free_dirs(dirs, file_ct);
                i++;
            }
            BYEL("Name ct: %i\n", name_ct);
            free_dirs(names, name_ct);          
        }


    } else if (strcmp(command, "remove") == 0) {
        BYEL("Uninstalling package...\n");
        Package* p = (Package*)malloc(sizeof(Package));
        int found = 0;
        if (scan_index(p, opt, index_path, row_length, 0, 16, &found) < 0) {
            BRED("Failed to scan index!\n");
            if (p) free(p);
            return -1;
        }

        if (!found) {
            printf("Package '%s' not installed!\n", opt);
            if (p) free(p);
            return 0;
        }

        if (delete_index(index_path, row_length, p->rowid) < 0) {
            BRED("Error removing package from index!\n");
            if (p) free(p);
            return -1;
        }

        if (p) free(p);

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

        head = packages;
        while (head != NULL) {
            Package* next = head->next;
            if (head) free(head);
            head = next;
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

        head = packages;
        while (head != NULL) {
            Package* next = head->next;
            if (head) free(head);
            head = next;
        }

    } else if (strcmp(command, "upload") == 0) {
        int status;
        if (argc != 4) {
            BRED("Invalid number of arguments for command upload!\n");
            return -1;
        }

        char* script = argv[2];
        char* signature = argv[3];

        char* package_ptr = NULL;

        printf("Script path: %s\n", script);
        printf("Signature: %s\n", signature);

        package_ptr = strrchr(script, '/');

        if (package_ptr == NULL) {
            BRED("Package name contains no '/'\n");
            return -1;
        }

        char* package_name = (char*) malloc(32);
        strncpy(package_name, package_ptr + 1, 32);

        printf("Package dir: %s\n", package_dir);
        printf("Package name: %s\n", package_name);

        if (!exists(script)) {
            BRED("Invalid script provided for upload!\n");
            free(package_name);
            return -1;
        }

        size_t sz = 0;
        bytes_t script_bin = read_bin(script, &sz);
        if (sz == 0) {
            BRED("Empty script provided for upload!\n");
            free(script_bin);
            free(package_name);
            return -1;
        }
        bytes_t write_bin = (bytes_t)calloc(sz + 64, sizeof(BYTE));
        BYTE sig[] = {"howdsdfsdfsdfsdfsdfsdf"};
        if (pad(sig, strlen((char*)sig), 64) < 0) {
            BRED("Failed to pad signature!\n");
            free(write_bin);
            free(script_bin);
            free(package_name);
            return -1;
        }

        memcpy(write_bin, sig, 64);
        memcpy(write_bin + 64, script_bin, sz);

        char* package_path = (char*)malloc(strlen(package_dir) + strlen(package_name) + 1);

        memcpy(package_path, package_dir, strlen(package_dir));
        memcpy(package_path + strlen(package_dir), package_name, strlen(package_name));
        package_path[strlen(package_dir) + strlen(package_name)] = 0;

        FILE* fp = fopen(package_path, "wb");

        if (!fp) {
            BRED("Failed to write to file: %s\n", package_path);
            if (package_path) free(package_path);
            if (script_bin) free(script_bin);
            if (write_bin) free(write_bin);
            free(package_name);
            return -1;
        }

        fwrite(write_bin, 1, sz + 64, fp);

    cleanup:
        fclose(fp);
        if (package_path) free(package_path);
        if (script_bin) free(script_bin);
        if (write_bin) free(write_bin);
        if (package_name) free(package_name);
       
    } else if (strcmp(command, "verify") == 0) {
        BMAG("Verifying package signatures...\n");

    } else if (strcmp(command, "registry") == 0) {
        char registry[] = "/usr/local/comet/.internal/registry/";
        char** files = NULL;
        int file_ct = 0;
        if (list_files(&files, registry, &file_ct) < 0) {
            BRED("Failed to list files for registry!\n");
            return -1;
        }

        int i = 0;
        printf("Registry\n=================================\n");
        while (i < file_ct) {
            if (files[i]) {
                char* str = (char*)malloc(64);
                if (last_of(str, files[i], '/') < 0) {
                    BRED("Error finding file path!\n");
                }
                str[strlen(str)] = 0;
                printf("\t%s\n", str);
                free(str);
                free(files[i]);
            }
            i++;
        }
        if (files) free(files);

    } else if (strcmp(command, "newuser") == 0) {
        BYEL("Creating new user...\n");
        if (argc != 3) {
            BRED("Invalid number of arguments provided for newuser!\n");
            return -1;
        }

        char* newuser = argv[2];
    } else if (strcmp(command, "update_test") == 0) {
        if (argc < 3) {
            BRED("Invalid number of arguments for update_test\n");
            return -1;
        }

        BYEL("Update package test...\n"); 
        int num_packages = 0;
        if (load_index(&packages, &num_packages, index_path, row_length) < 0) {
            BRED("Failed to load index!\n");
            return -1;
        }
        Package* head = packages;
        while (head != NULL) {
            if (strcmp(head->name, opt) == 0) {
                head->version = 102;
                head->updated_at = (long)time(NULL);
                head->created_at = (long)0;
                memcpy(head->script, "hate_my_life", 14);
                head->script[14] = 0;
                printf("Headache creation: %li\n", head->created_at);
                printf("Headache updation: %li\n", head->updated_at);
                if (update_index_package(head, index_path, row_length) < 0) {
                    BRED("Failed to update index by package!\n");
                    return -1;
                }
            }
            head = head->next;
        }

        head = packages;
        while (head != NULL) {
            Package* next = head->next;
            free(head);
            head = next;
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

        // if (create_dir("/usr/local/comet/compress") < 0) {
        //     BRED("Error initing comet dirs\n"); return -1;
        // }

        // if (chmod("/usr/local/comet/compress", 0755) == -1) {
        //     printf("Error setting executable bit on temporary file.\n");
        //     return 1;
        // }

        if (create_dir("/usr/local/comet/env") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/env", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/.internal") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/.internal", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/.internal/lib") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/.internal/lib", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/.internal/registry") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/.internal/registry", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/.internal/cache") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/.internal/cache", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

        if (create_dir("/usr/local/comet/.internal/env") < 0) {
            BRED("Error initing comet dirs\n"); return -1;
        }

        if (chmod("/usr/local/comet/.internal/env", 0755) == -1) {
            printf("Error setting executable bit on temporary file.\n");
            return 1;
        }

    } else {
        BRED("Invalid command provided!\n");
    }

    BGRE("Gracefully terminated!\n");
    return 0;
}