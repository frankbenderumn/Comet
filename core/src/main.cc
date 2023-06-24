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

#include "comet/utils.h"
#include "comet/command/commands.h"

const char* TOKEN_PATH = "./config/tokens";

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

    } else if (strcmp(command, "install") == 0) {
        
        if (install(packages, opt, index_path, row_length) < 0) {
            BRED("Failed to install!\n");
            return -1;
        }

    } else if (strcmp(command, "help") == 0 || strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
        printf("======================================================================================================\n");
        printf("                                       Comet Package Manager                                          \n");
        printf("======================================================================================================\n");
        printf("%-32s: %s\n", "install", "Install a package within the registry");   
        printf("%-32s: %s\n", "uninstall", "Uninstall a package");   
        printf("%-32s: %s\n", "uninstall -a", "Uninstall all packages");   
        printf("%-32s: %s\n", "list", "List all packages with detailed information");   
        printf("%-32s: %s\n", "upload <script> <deps> <user>", "Upload a package to the registry with given dependencies for a user");   
        printf("%-32s: %s\n", "show <package_name>", "Get detailed information about a package");
        // printf("%-32s: %s\n", "update <package_name> <new_script>", "Update package with a new script");
        printf("%-32s: %s\n", "load", "Reads package names from a comet.txt file to install required packages");
        printf("%-32s: %s\n", "purge <package>", "Removes a package and its cached build!\n");
        printf("%-32s: %s\n", "purge -a/--all", "(Not recommended) Will clear all package installations (cached and environmental). Will require password.\n");

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

    } else if (strcmp(command, "upload") == 0) {

        if (upload(argc, argv) < 0) {
            BRED("Failed to upload!\n");
            return -1;
        }

    } else if (strcmp(command, "script") == 0) {
        if (argc != 4) {
            BRED("Invalid number of arguments provided for command script!\n");
            return -1;
        }

        char* package_name = argv[2];
        char* script_command = argv[3];

        char script_dir[] = "./.internal/package/";

        char* package_path = (char*)malloc(strlen(package_name) + strlen(script_dir) + 1);

        if (package_path) free(package_path);
        if (sig) free(sig);
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

    } else if (strcmp(command, "purge") == 0) {
        if (argc < 3) {
            BRED("Invalid number of arguments provided!\n");
            return -1;
        }

        int file_ct = 0;
        char** dirs = load_dirs(dirs, comet_dir, opt, &file_ct);
        size_t lib_sz = 64;

        int arr[] = {5, 6, 7, 8, 10, 11};
        int arr_size = sizeof(arr)/sizeof(int);

        int has_libs = 1;
        FILE* fp = fopen(dirs[5], "rb");
        if (!fp) {
            has_libs = 0;
        }

        if (has_libs) {
            size_t lib_file_sz = fsize(fp);
            BYEL("Lib file sz: %li\n", lib_file_sz);
            char buffer[lib_sz];
            while (fread(buffer, 1, lib_sz, fp) == lib_sz) {
                char name[strlen(buffer) + 1];
                memcpy(name, buffer, strlen(buffer));
                name[strlen(buffer)] = 0;
                printf("Lib name: %s\n", name);
                char* main_lib = (char*)malloc(strlen(dirs[3]) + strlen(name) + 1);
                memcpy(main_lib, dirs[3], strlen(dirs[3]));
                memcpy(main_lib + strlen(dirs[3]), name, strlen(name));
                main_lib[strlen(dirs[3]) + strlen(name)] = 0;
                printf("Main lib file: %s\n", main_lib);
                if (fremove(main_lib) < 0) {
                    BRED("Failed to remove library '%s'\n", main_lib);
                } 
                free(main_lib);
            }
        }

        fclose(fp);

        for (int i = 0; i < arr_size; i++) {
            if (exists(dirs[arr[i]])) {
                if (path_type(dirs[arr[i]]) == 0) {
                    int status = remove(dirs[arr[i]]);
                    if (status == 0) {
                        printf("File removed '%s' successfully.\n", dirs[arr[i]]);
                    } else {
                        printf("Error removing file '%s'.\n", dirs[arr[i]]);
                    }
                } else if (path_type(dirs[arr[i]]) == 1) {
                    if (remove_dir_r(dirs[arr[i]]) < 0) {
                        BRED("Failed to remove directory '%s'\n", dirs[arr[i]]);
                    }
                }
            }
        }

        Package* p = (Package*)malloc(sizeof(Package));
        int found = 0;
        if (scan_index(p, opt, index_path, row_length, 0, 16, &found) < 0) {
            BRED("Error scanning index!\n");
            if (p) free(p);
            free_dirs(dirs, file_ct);
            return -1;
        }

        if (delete_index(index_path, row_length, p->rowid) < 0) {
            BRED("Error removing package from index!\n");
            if (p) free(p);
            free_dirs(dirs, file_ct);
            return -1;
        }

        if (p) free(p);
        free_dirs(dirs, file_ct);

    } else if (strcmp(command, "newuser") == 0) {
        BYEL("Creating new user...\n");
        if (argc != 3) {
            BRED("Invalid number of arguments provided for newuser!\n");
            return -1;
        }

        char* newuser = argv[2];

    } else if (strcmp(command, "token") == 0) {
        if (argc != 4) {
            BRED("Invalid number of arguments provided for command 'token'. Usage: comet token <key> <value>\n");
            return -1;
        }

        char* key = argv[2];
        char* val = argv[3];
        char toWrite[strlen(argv[2]) + strlen(argv[3]) + 2];

        #define MAX_ITEMS 100
        #define MAX_KEY_LENGTH 50

        typedef struct {
            char key[MAX_KEY_LENGTH];
            int count;
        } KeyCount;

        KeyCount keyCounts[MAX_ITEMS];
        int itemCount = 0;

        if (access(TOKEN_PATH, F_OK) != -1) {

            FILE* fp_read;
            if ((fp_read = fopen(TOKEN_PATH, "r")) == NULL) {
                BRED("Failed to open file '%s' for reading\n", TOKEN_PATH);
            }

            char line[100];
            while (fgets(line, sizeof(line), fp_read) != NULL) {
                line[strcspn(line, "\n")] = '\0'; // Remove trailing newline character
                // printf("Line: %s\n", line);
                int eq_count = 0;
                for (int i = 0; line[i] != '\0'; i++) {
                    if (line[i] == '=') eq_count++;
                }
                if (eq_count > 1) {
                    BRED("Too many equal characters token in token!\n");
                    fclose(fp_read);
                    exit(1);
                }
                char* p = strstr(line, "=");
                if (p == NULL) {
                    BRED("Corrupted data detected in token file. No = found in line!\n");
                    fclose(fp_read);
                    exit(1);
                }
                char* key = strtok(line, "=");
                char* value = strtok(NULL, "=");

                // if (key != NULL && value != NULL) {
                //     printf("Key: %s\n", key);
                //     printf("Value: %s\n", value);
                // }

                // Find if the key already exists in the keyCounts array
                int keyIndex = -1;
                for (int i = 0; i < itemCount; i++) {
                    if (strcmp(keyCounts[i].key, key) == 0) {
                        BRED("Key '%s' already exists! Please use update_token instead!\n", key);
                        exit(1);
                        keyIndex = i;
                        break;
                    }
                }

                // Update the count for an existing key or add a new key
                if (keyIndex != -1) {
                    keyCounts[keyIndex].count++;
                } else if (itemCount < MAX_ITEMS) {
                    strcpy(keyCounts[itemCount].key, key);
                    keyCounts[itemCount].count = 1;
                    itemCount++;
                } else {
                    printf("Reached the maximum number of items.\n");
                    break;
                }
            }
            fclose(fp_read);

        }

        FILE* fp;
        if ((fp = fopen(TOKEN_PATH, "a")) == NULL) {
            BRED("Failed to open file '%s'\n", TOKEN_PATH);
            return -1;
        }

        int keyIndex = -1;
        for (int i = 0; i < itemCount; i++) {
            if (strcmp(keyCounts[i].key, key) == 0) {
                BRED("Key '%s' already exists! Please use update_token instead!\n", key);
                fclose(fp);
                exit(1);
                keyIndex = i;
                break;
            }
        }

        memcpy(toWrite, key, strlen(key));
        memcpy(toWrite + strlen(key), "=", 1);
        memcpy(toWrite + strlen(key) + 1, val, strlen(val));
        toWrite[strlen(key) + strlen(val) + 1] = 0;

        fprintf(fp, "%s\n", toWrite);

        fclose(fp);

    } else if (strcmp(command, "update_token") == 0) {

        if (argc != 4) {
            BRED("Invalid number of arguments provided for command 'token'. Usage: comet update_token <key> <value>\n");
            return -1;
        }

        char* key = argv[2];
        char* val = argv[3];

        if (access(TOKEN_PATH, F_OK) != -1) {

            FILE* fp_read;
            if ((fp_read = fopen(TOKEN_PATH, "r")) == NULL) {
                BRED("Failed to open file '%s' for reading\n", TOKEN_PATH);
            }

            // Create a temporary file to write the updated contents
            const char* temp_file_path = "./config/token_temp";
            FILE* temp_file = fopen(temp_file_path, "w");
            if (temp_file == NULL) {
                printf("Failed to create temporary file.\n");
                fclose(fp_read);
                return 1;
            }

            char line[100];
            while (fgets(line, sizeof(line), fp_read) != NULL) {
                char* parsed_key = strtok(line, "=");
                char* parsed_value = strtok(NULL, "=");

                if (parsed_key != NULL && parsed_value != NULL) {
                    // printf("Key: %s\n", parsed_key);
                    // printf("Value: %s\n", parsed_value);
                    if (strcmp(parsed_key, key) == 0) {
                        // MAG("Let's update key!\n");
                        fprintf(temp_file, "%s=%s\n", parsed_key, val);
                    } else {
                        fprintf(temp_file, "%s=%s", parsed_key, parsed_value);
                    }
                }

            }

            fclose(fp_read);
            fclose(temp_file);

            // Replace the original file with the updated file
            if (remove(TOKEN_PATH) != 0) {
                printf("Failed to remove original file.\n");
                return 1;
            }
            if (rename(temp_file_path, TOKEN_PATH) != 0) {
                printf("Failed to rename temporary file.\n");
                return 1;
            }

        } else {
            BRED("Token file has not been initiated!\n");
            return 1;
        }

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