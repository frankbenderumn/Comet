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