#include <fstream>

#include <dirent.h>
#include <string.h>

#include "comet/util/io.h"

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
                (*files)[i] = (char*)malloc(strlen(directory) + strlen(ent->d_name) + 2);
                memcpy((*files)[i], directory, strlen(directory));
                memcpy((*files)[i] + strlen(directory), "/", 1);
                memcpy((*files)[i] + strlen(directory) + 1, ent->d_name, strlen(ent->d_name));
                (*files)[i][strlen(directory) + strlen(ent->d_name) + 1] = 0;
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

std::string fread(const char* path) {
    std::string envfile_path(path);
    std::ifstream inputFile(envfile_path, std::ios::binary);

    if (!inputFile.is_open()) {
        BRED("Error opening file: %s\n", envfile_path.c_str());
        return ""; // Return an empty string to indicate an error
    }

    inputFile.seekg(0, std::ios::end);
    std::streampos env_sz = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::string fileContent;
    fileContent.resize(env_sz);

    // Read the file content into the string
    inputFile.read(&fileContent[0], env_sz);

    // Close the file and cleanup list malloc
    inputFile.close();

    return fileContent;
}

int fremove(const char* path) {
    int status = remove(path);
    if (status == 0) {
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
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) {
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
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char* filepath = (char*)malloc(
            strlen(path) + strlen(entry->d_name) + 2
        );
        
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

PathType path_type(const char* path) {
    struct stat path_stat;
    if (stat(path, &path_stat) == 0) {
        if (S_ISREG(path_stat.st_mode)) {
            return PATH_FILE;
        } else if (S_ISDIR(path_stat.st_mode)) {
            return PATH_DIR;
        } else {
            return PATH_UNKNOWN;
        }
    } else {
        perror("Error");
        return PATH_UNKNOWN;
    }
}