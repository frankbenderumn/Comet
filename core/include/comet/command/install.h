#ifndef COMET_COMMAND_INSTALL_H_ 
#define COMET_COMMAND

#include "comet/index.h"

int install(Package* packages, char* opt, const char* index_path, int row_length) {
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
        BRED("No uploaded package found for name '%s'\n", opt);
        if (access(s.c_str(), F_OK) != -1) {
            printf("File exists.\n");
            char** passables = (char**)malloc(sizeof(char*) * 4);
            passables[0] = strdup("sh");
            passables[1] = strdup("install");
            passables[2] = strdup("placeholder");
            passables[3] = 0;

            if (chmod(s.c_str(), 0755) == -1) {
                printf("Error setting executable bit on temporary file.\n");
                free(passables[0]);
                free(passables[1]);
                free(passables[2]);
                free(passables);
                return 1;
            }

            int status;
            if ((status = pipe(s.c_str(), &passables)) < 0) {
                BRED("Failed to pipe command '%s'\n", s.c_str());
                free(passables[0]);
                free(passables[1]);
                free(passables[2]);
                free(passables);
                status = -1;
                exit(EXIT_FAILURE);
                // return -1;
            }

            free(passables[0]);
            free(passables[1]);
            free(passables[2]);
            free(passables);

        } else {
            printf("File does not exists.\n");
        }
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
        // fprintf(stderr, "Error: failed to open input file %s\n", argv[1]);
        return 1;
    }

    FILE* output = fopen(exec_path, "wb");
    if (!output) {
        // fprintf(stderr, "Error: failed to open output file %s\n", argv[2]);
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

    return 1;
}

#endif