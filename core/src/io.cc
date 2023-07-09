#include <dirent.h>
#include <string.h>

#include "comet/io.h"


// dos2unix function
//     FILE* input = fopen(temp_path, "rb");
//     if (!input) {
//         // fprintf(stderr, "Error: failed to open input file %s\n", argv[1]);
//         return 1;
//     }

//     FILE* output = fopen(exec_path, "wb");
//     if (!output) {
//         // fprintf(stderr, "Error: failed to open output file %s\n", argv[2]);
//         fclose(input);
//         return 1;
//     }

//     // dos2unix install script
//     int ch;
//     int last_ch = '\n';
//     while ((ch = fgetc(input)) != EOF) {
//         if (ch == '\r') {
//             // Replace CRLF with LF
//             if (last_ch == '\n') {
//                 fseek(output, -1, SEEK_CUR);
//             }
//             ch = '\n';
//         }
//         fputc(ch, output);
//         last_ch = ch;
//     }

//     fclose(input);
//     fclose(output);

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

int path_type(const char* path) {
    struct stat path_stat;
    
    if (stat(path, &path_stat) == 0) {
        if (S_ISREG(path_stat.st_mode)) {
            printf("%s is a regular file.\n", path);
            return 0;
        } else if (S_ISDIR(path_stat.st_mode)) {
            printf("%s is a directory.\n", path);
            return 1;
        } else {
            printf("%s is neither a regular file nor a directory.\n", path);
            return 2;
        }
    } else {
        printf("Error: Unable to determine the type of %s.\n", path);
        return -1;
    }
    return -1;
}

int fremove(const char* path) {
    int status = remove(path);
    if (status == 0) {
        printf("File removed '%s' successfully.\n", path);
        return 0;
    } else {
        printf("Error removing file '%s'.\n", path);
        return -1;
    }
    return -1;
}

long fsize(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);
    return sz;
}


int has_deps(const char* path) {
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

int pad(bytes_t bytes, size_t bytes_len, size_t width) {
    if (bytes_len >= width) {
        // No padding necessary
        return 0;
    }
    
    bytes_t padded_bytes = (bytes_t) malloc(width * sizeof(BYTE));
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

void free_dirs(char** dirs, int sz) {
    int i = 0;
    while (i < sz) {
        // printf("Freeing dirs[%d]: %p\n", i, (void*)dirs[i]);
        if (dirs[i]) free(dirs[i]);
        i++;
    }
    free(dirs);
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
        "lib/", 
        ".internal/registry/", 
        ".internal/lib/", 
        "source/", 
        "include/", 
        ".internal/lib/", 
        "build/", 
        "deps/", 
        // "sub/",
        "bin/"
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
            if (strcmp(extensions[i-1], "source/") == 0) {
                int path_len = strlen(comet_dir) + strlen(extensions[i-1]) + strlen(package_name) + strlen("-source") + 1;
                path = (char*)malloc(path_len);
                snprintf(path, path_len, "%s%s%s%s", comet_dir, extensions[i-1], package_name, "-source");
            } else {
                int path_len = strlen(comet_dir) + strlen(extensions[i-1]) + strlen(package_name) + 1;
                path = (char*)malloc(path_len);
                snprintf(path, path_len, "%s%s%s", comet_dir, extensions[i-1], package_name);
            }
        }

        dirs[i] = path;
        BYEL("[%i] file_path: %s\n", i, dirs[i]);
    }
    
    return dirs;
}