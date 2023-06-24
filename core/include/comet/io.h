#ifndef COMET_IO_H_
#define COMET_IO_H_ 

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "comet/color.h"

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

#endif