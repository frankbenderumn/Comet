#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "comet/defs.h"
#include "comet/io.h"
#include "comet/color.h"
#include "comet/index.h"
#include "comet/package.h"
#include "comet/manifest.h"

int index_meta(char** index_path, int* row_length) {
    const char* comet_dir = getenv("COMET_DIR");
    if (comet_dir == NULL) {
        printf("\033[1;31mError:\033[0m Failed to detect env var COMET_DIR\n");
        return -2;
    }
    size_t index_path_sz = strlen(comet_dir) + strlen("/.internal/index_ascii");
    *index_path = (char*)realloc(*index_path, index_path_sz + 1);
    if (*index_path == NULL) {
        printf("\033[1;31mError:\033[0m Failed to allocate memory for index_path\n");
        return -3;
    }
    snprintf(*index_path, index_path_sz + 1, "%s%s", comet_dir, "/.internal/index_ascii");
    FILE* fp = fopen(*index_path, "rb");
    if (fp != NULL) {
        fread(row_length, sizeof(int), 1, fp);
    } else {
        printf("\033[1;35mWarning:\033[0m Index file '%s' not found!\n", *index_path);
        return -1;
    }
    fclose(fp);
    return 1;
}

int index_load(Package*** packages, int* row_length) {
    char* index_path = 0;
    int parsed_row_length = 0;
    Package* package = 0;

    if (index_meta(&index_path, &parsed_row_length) < 0) {}

    FILE* fp = fopen(index_path, "rb");
    if (fp == NULL) {
        printf("\033[1;35mWarning:\033[0m Index file '%s' not found\n", index_path);
        free(index_path);
        return 0;
    }

    size_t header = 0;
    fread(&header, sizeof(size_t), 1, fp);

    BLU("(Header) Record length: %li\n", header);

    unsigned char buffer[344];
    int j = 0;

    int sz = fsize(fp);

    BLU("File size: %i\n", sz);
    BLU("Num records: %i\n", (sz - sizeof(size_t)) / header);

    if (sz < 16) {
        return 0;
    }

    fseek(fp, 8, SEEK_SET);
    while (fread(buffer, 1, parsed_row_length, fp) == parsed_row_length) {
        BLU("Buffer: %s\n", buffer);
        PackageManifest* manifest = (PackageManifest*)calloc(1, sizeof(PackageManifest));
        SemVer* sv = (SemVer*)calloc(1, sizeof(SemVer));
        package = (Package*)calloc(1, sizeof(Package));
        manifest->versions_sz = 0;
        manifest->deps_sz = 0;

        // dry up to separate function? (DTO?)
        MemMap attrs[] = {
            {manifest->name, COMET_STR, sizeof(manifest->name)},
            {manifest->source, COMET_STR, sizeof(manifest->source)},
            {manifest->uri, COMET_STR, sizeof(manifest->uri)},
            {manifest->repo, COMET_STR, sizeof(manifest->repo)},
            {sv->string, COMET_STR, sizeof(sv->string)},
            {&package->created_at, COMET_TS, sizeof(package->created_at)},
            {&package->updated_at, COMET_TS, sizeof(package->updated_at)},
            {&manifest->versions_sz, COMET_INT, sizeof(manifest->versions_sz)},
            {&manifest->deps_sz, COMET_INT, sizeof(manifest->deps_sz)},
            {package->signature, COMET_STR, sizeof(package->signature)}
        };

        int lando = sizeof(attrs) / sizeof(MemMap);
        BRED("Lando is: %i\n", lando);

        // // Calculate the total size needed for serialization
        int sz = 0;
        for (int i = 0; i < lando; i++) {
            sz += attrs[i].size;
        }
        *row_length = sz;

        int i = 0;
        int offset = 0;
        while (i < lando) {
            memcpy(attrs[i].addr, buffer + offset, attrs[i].size);
            if (attrs[i].type == COMET_STR) {
                ((char*)attrs[i].addr)[attrs[i].size - 1] = '\0';
            }
            offset += attrs[i].size;
            i++;
        }

        YEL("%s\n", manifest->name);

        YEL("Created_at: %li\n", package->created_at);
        YEL("Updated_at: %li\n", package->updated_at);


        // strncpy(package->manifest->latest->string, sv->string, strlen(sv->string));
        manifest->latest = sv;
        package->manifest = manifest;
        package->row_id = j;
        *packages = (Package**)realloc(*packages, sizeof(Package*) * (j + 1));
        (*packages)[j] = package;

        manifest_dump((*packages)[j]->manifest);
        package_show((*packages)[j]);

        j++;
    
    }

    fclose(fp);

    free(index_path);
    GRE("Deserialization finished\033[0m\n");
    return j;
}

int index_add(Package*** packages, int packages_sz, PackageManifest* manifest) {
    char* index_path = 0;
    int row_length = 0;

    GRE("Guantanamo!\n");

    // if index does not exist then index_meta will return -1.
    // The code then executed will created the index file as it does not exist
    if (index_meta(&index_path, &row_length) < 0) {

        Package* p = package_create(manifest);

        size_t serialized_sz = 0;
        unsigned char serialized[344]; // need to find a way to not hard code this successfully.
        if (package_serialize(p, serialized, &serialized_sz) < 0) {
            BRED("Index add: Failed to serialize package!");
            free(p);
            free(index_path);
            return 0;
        }

        BLU("Serialized sz is: %li\n", serialized_sz);

        p->row_id = 1;

        FILE* fp = NULL;
        
        fp = fopen(index_path, "w");
        if (fp == NULL) {
            BRED("Index add: Error opening file: %s\n", index_path);
            free(p);
            free(index_path);
            return 0;
        }

        fwrite(&serialized_sz, sizeof(size_t), 1, fp);
        fwrite(serialized, 1, serialized_sz, fp);

        fclose(fp);

        // free(serialized);
        free(index_path);

        (*packages)[0] = p;

        package_show((*packages)[0]);

        BGRE("Index created!\n");
        return 1;

    } else {

        BMAG("Why does everything with this have to be a complete bitch and a half: %i\n", packages_sz);
        BMAG("================================================================\n");

        *packages = (Package**)realloc(*packages, sizeof(Package*) * (packages_sz + 1));

        (*packages)[packages_sz] = package_create(manifest);

        // packages_list(*packages, *packages_sz + 1);

        (*packages)[packages_sz]->row_id = packages_sz + 1;

        FILE* fp = fopen(index_path, "ab");

        int index_sz = fsize(fp);

        MAG("Before append index file size is: %i\n", index_sz);

        if (fp == NULL) {
            printf("\033[1;35Internal:\033[0m Failed to open index file '%s' for addition\n", index_path);
            return packages_sz;
        }

        // for (int i = 0; i < packages_sz + 1; i++) {
            unsigned char bytes[344];
            size_t bytes_sz = 0;
            if (package_serialize((*packages)[packages_sz], bytes, &bytes_sz) < 0) {
                // break;
            }

            printf("index_add: bytes_sz is '%i'\n", bytes_sz);

            package_show((*packages)[packages_sz]);

            fwrite(bytes, 1, row_length, fp);

            // free(bytes);
        // }

        int sz = fsize(fp);
        BCYA("======================================================\n\n");
        CYA("index_add: File size of new index is: %i\n\n", sz);
        BCYA("======================================================\n\n");

        // (*packages_sz)++;

        fclose(fp);
        free(index_path);

        return packages_sz + 1;

    }
}

int index_remove(Package** packages, int packages_sz, char* name) {
    int to_delete = -1;
    for (int i = 0; i < packages_sz; i++) {
        if (strcmp(packages[i]->manifest->name, name) == 0) {
            to_delete = i;
        }
    }

    char* index_path = 0;
    int row_length = 0;
    if (index_meta(&index_path, &row_length) < 0) {}

    char* index_path_temp = (char*)malloc(strlen(index_path) + 5 + 1);
    strncpy(index_path_temp, index_path, strlen(index_path));
    strncpy(index_path_temp + strlen(index_path), "_temp", 5);
    index_path_temp[strlen(index_path) + 5] = '\0'; 

    if (to_delete > -1) {
        FILE* read = fopen(index_path, "rb");
        FILE* write = fopen(index_path_temp, "wb");

        if (read == NULL) {
            printf("\033[1;31m\033[0m Failed to open index file '%s'!\n", index_path);
            free(index_path);
            free(index_path_temp);
            return -1;
        }

        if (write == NULL) {
            printf("\033[1;31m\033[0m Failed to open index temp file '%s'!\n", index_path_temp);
            free(index_path);
            free(index_path_temp);
            return -1;
        }

        if (write == NULL) { return -1; }
        size_t header = 0;
        fread(&header, sizeof(size_t), 1, read);
        fclose(read);

        fwrite(&header, sizeof(size_t), 1, write);
        for (int i = 0; i < packages_sz; i++) {
            if (i != to_delete) {
                unsigned char serialized[344];
                size_t serialized_sz = 0;
                if (package_serialize(packages[i], serialized, &serialized_sz) < 0) {
                    printf("\033[1;31m\033[0m Failed to serialize package '%s'!\n", packages[i]->manifest->name);      
                    fclose(write);
                    free(index_path);
                    free(index_path_temp);              
                    return -1;
                }
                
                fwrite(serialized, 1, serialized_sz, write);
            }
        }


        if (fsize(write) < 25) {

            fremove(index_path);
            fremove(index_path_temp);

            fclose(write);

            free(index_path_temp);
            free(index_path);

            return 0;
        }

        fclose(write);

    }

    int status = fremove(index_path);

    // Renaming the file
    int result = rename(index_path_temp, index_path);

    if (result == 0) {
        printf("File renamed successfully.\n");
    } else {
        perror("Error renaming file");
        free(index_path_temp);
        free(index_path);
        return -1;
    }

    free(index_path_temp);
    free(index_path);

    return 0;
}


void index_dump(Package** packages, int packages_sz) {
    for (int i = 0; i < packages_sz; i++) {
        package_show(packages[i]);
    }
}

int index_update(Package*** packages, int packages_sz, PackageManifest* manifest) {
    char* index_path = 0;
    int row_length = 0;
    if (index_meta(&index_path, &row_length) < 0) {}

    char* index_path_temp = (char*)malloc(strlen(index_path) + 5 + 1);
    strncpy(index_path_temp, index_path, strlen(index_path));
    strncpy(index_path_temp + strlen(index_path), "_temp", 5);
    index_path_temp[strlen(index_path) + 5] = '\0'; 

    FILE* read = fopen(index_path, "rb");
    FILE* write = fopen(index_path_temp, "wb");

    if (read == NULL) {
        printf("\033[1;31m\033[0m Failed to open index file '%s'!\n", index_path);
        free(index_path);
        free(index_path_temp);
        return -1;
    }

    if (read == NULL) {
        printf("\033[1;31m\033[0m Failed to open index file '%s'!\n", index_path);
        free(index_path);
        free(index_path_temp);
        return -1;
    }

    size_t header = 0;
    fread(&header, sizeof(size_t), 1, read);
    fwrite(&header, sizeof(size_t), 1, write);
    fclose(read);

    RED("Header value: %li\n", header);

    for (int i = 0; i < packages_sz; i++) {
        BCYA("Created_at: %li\n", (*packages)[i]->created_at);
        BCYA("Updated_at: %li\n", (*packages)[i]->updated_at);
        
        if (strcmp((*packages)[i]->manifest->name, manifest->name) == 0) {
            (*packages)[i]->updated_at = time(NULL);
        }
        package_show((*packages[i]));

        BMAG("Created_at: %li\n", (*packages)[i]->created_at);
        BMAG("Updated_at: %li\n", (*packages)[i]->updated_at);

        size_t serialized_sz = 0;
        unsigned char serialized[344];
        if (package_serialize(*packages[i], serialized, &serialized_sz) < 0) {
            printf("\033[1;31mError:\033[0m Failed to serialize package during update!\n");
        }
        fwrite(serialized, 1, serialized_sz, write);
    }

    fclose(write);

    if (fremove(index_path) < 0) {
        printf("\033[1;31mError:\033[0m Failed to update index file!\n");
        free(index_path_temp);
        free(index_path);
        return -1;
    }

    int status = rename(index_path_temp, index_path);

    if (status == 0) {
        printf("File renamed successfully.\n");
    } else {
        perror("Error renaming file");
        free(index_path_temp);
        free(index_path);
        return -1;
    }


    free(index_path);
    free(index_path_temp);

}

int index_verify(Package*** packages, int packages_sz, const char* package_name) {
    BLU("Verifying index with packages...\n");
    bool found = false;
    PackageManifest* target = NULL;
    for (int i = 0; i < packages_sz; i++) {
        if (strcmp((*packages)[i]->manifest->name, package_name) == 0) {
            found = true;
            target = (*packages)[i]->manifest;
            break;
        }
    }

    if (!found) {
        BLU("sz: %li\n", packages_sz);
        PackageManifest** manifest = NULL;
        int manifest_sz = manifest_load(&manifest, "./manifest/manifest.json");
        target = manifest_get_by_name(manifest, manifest_sz, package_name);
        if (index_add(packages, packages_sz, target) < 0) {
            printf("\033[1;31mError:\033[0m Failed to add package '%s' to index!\n", target->name);
            manifest_array_destruct(&manifest, manifest_sz);
            // if (target) free(target);
            return -1;
        }
        if ((*packages)[0]) free((*packages)[0]);
        manifest_array_destruct(&manifest, manifest_sz);
    }

    // if (target) free(target);

    return 0;
}

void index_alter(Package** index, Package** new_packages) {
    const char* comet_dir = getenv("COMET_DIR");
    const size_t index_path_sz = strlen(comet_dir) + strlen("/.internal/index");
    char index_path[strlen(comet_dir) + strlen("/.internal/index") + 1];
    snprintf(index_path, index_path_sz + 1, "%s%s", comet_dir, "/.internal/index");

    Package* index_head = *index;
    Package* package_head = *new_packages;
    Package** ptr = NULL;
    int match = 0;
    int old_len = 0;
    while (package_head != NULL) {
        while (index_head != NULL) {
            // BMAG("%s (%li) == %s (%li)\n", package_head->name, sizeof(package_head->name), index_head->name, sizeof(index_head->name));
            // if (strcmp(index_head->name, package_head->name) == 0) {
            //     BGRE("INDEX MATCH detected!\n");
            //     if (index_verify(package_head, index_head) < 0) {
            //         return;
            //     }
            //     match = 1;
            // }
            // index_head = index_head->next;
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
        // package_head = package_head->next;
        index_head = *index;
    }
    if (match == 0) {
        BRED("No index match detected!\n");
    }
    printf("Length of toAdd is: %i\n", old_len);
    for (int i = 0; i < old_len; i++) {
        // printf("Package: %s\n", ptr[i]->name);
        // printf("Version: %i\n", ptr[i]->major_version);
    }

    if (old_len > 0) {
        if (exists(index_path)) {
            // index_add(ptr, old_len);
        } else {
            // index_create(ptr, old_len, index_path, row_length);
        }
    }

    free(ptr);
}

int index_delete(int row_id) {
    char* index_path = 0;
    int row_length = 0;
    if (index_meta(&index_path, &row_length) < 0) {
        return -1;
    }

    if (!exists(index_path)) {
        BRED("Index not created!\n");
        return -1;
    }

    long sz = 0;
    bytes_t data = read_bin(index_path, &sz);

    BRED("Mag size of index is: %li\n", sz);
    BRED("Num records is: %li\n", sz / row_length);

    bytes_t new_index = (bytes_t)malloc(sizeof(BYTE) * (sz - row_length));

    if (row_id == 0) {
        memcpy(new_index, data + row_length, sz - row_length);
    } else if (row_id == ((sz / row_length) - 1)) {
        memcpy(new_index, data, sz - row_length);
    } else {
        // lets row_id is 5, row length is 100, total bytes is 700 
        // read the first 500 bytes
        memcpy(new_index, data, row_id * row_length);
        // read last 100 bytes
        memcpy(new_index + (row_id * row_length), data + ((row_id + 1) * row_length), sz - ((row_id + 1) * row_length));
    }

    BMAG("Verify bytes count: %li\n", (row_id * row_length) + (sz - ((row_id + 1) * row_length)));
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

int index_scan(Package* p, const char* id, int start, int end, int* found) {
    char* index_path = 0;
    int row_length = 0;
    if (index_meta(&index_path, &row_length) < 0) {
        return -1;
    }

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

        }
        j++;
    }
    BMAG("Done scanning!\n");
    return 0;
}