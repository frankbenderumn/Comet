#include "comet/commands.h"

#include <string>

#include "comet/util/common.h"

void env_list(const char* COMET_DIR) {

    // retrieves all file/dir names of directory, specifically COMET_DIR path
    int file_ct = 0;
    char** files = (char**)malloc(sizeof(char*));

    list_files(&files, (char*)COMET_DIR, &file_ct);

    // filters out internal files and directories. Removes shared prefix
    // to retrieve simply the name of the environment rather than path
    std::vector<std::string> envs;
    for (int i = 0; i < file_ct; i++) {
        std::string prefix = std::string(COMET_DIR) + "/";
        char* path = remove_common_prefix(files[i], prefix.c_str());
        if (strcmp(path, ".internal") != 0 && strcmp(path, "envfile") != 0) {
            envs.push_back(std::string(path));
        }
        free(path);
        free(files[i]);
    }

    free(files);

    // prints out environments
    printf("Environments\n");
    printf("=================================\n");
    for (const auto& e : envs) {
        printf("%s\n", e.c_str());
        printf("-----------------------------------\n");
    }

}

void env_show(const char* COMET_DIR) {

    // reads the name of current environment from envfile and removes \n char
    std::string envfile_path = std::string(COMET_DIR) + "/envfile";
    std::string env = fread(envfile_path.c_str());
    env.pop_back();

    printf("Environment: %s\n", env.c_str());

}

void env_use(const char* COMET_DIR, const char* OPTION) {

    // similar to env_list parses out environment names
    int file_ct = 0;
    char** files = (char**)malloc(sizeof(char*));

    list_files(&files, (char*)COMET_DIR, &file_ct);

    std::string found;
    for (int i = 0; i < file_ct; i++) {
        std::string prefix = std::string(COMET_DIR) + "/";
        char* pre = remove_common_prefix(files[i], prefix.c_str());
        if (strcmp(pre, OPTION) == 0) {
            found = std::string(pre) + "\n";
        }
        free(pre);
        free(files[i]);
    }

    free(files);

    // if environment name input (OPTION) exists, this name is written to
    // envfile to denote current used environment
    if (!found.empty()) {
        std::string efile = std::string(COMET_DIR) + "/envfile";
        FILE* fp = fopen(efile.c_str(), "wb");
        if (!fp) {
            BRED("Failed to open envfile for write!\n");
            return;
        }
        fwrite(found.data(), 1, found.size(), fp);
        fclose(fp);
    }

}

void env_create(const char* COMET_DIR, const char* OPTION) {

    // creates root directory for environment
    std::string s(COMET_DIR);
    std::string new_env = s + "/" + std::string(OPTION);

    create_dir(new_env.c_str());

    // creates subdirectories needed within environment
    std::string db = s + "/" + std::string(OPTION) + "/db";
    std::string bin = s + "/" + std::string(OPTION) + "/bin";
    std::string lib = s + "/" + std::string(OPTION) + "/lib";
    std::string include = s + "/" + std::string(OPTION) + "/include";
    std::string script = s + "/" + std::string(OPTION) + "/script";

    create_dir(db.c_str());
    create_dir(bin.c_str());
    create_dir(lib.c_str());
    create_dir(include.c_str());
    create_dir(script.c_str());

    // after creating environment, environment is selected as current,
    /** TODO: can probably just call env_show here */
    std::string envfile_path = s + "/envfile";
    FILE* fp = fopen(envfile_path.c_str(), "wb");

    if (!fp) {
        BRED("Failed to open envfile: %s\n", envfile_path.c_str());
        return;
    }

    std::string to_write = std::string(OPTION) + "\n";
    fwrite(to_write.data(), 1, to_write.size(), fp);

    fclose(fp);

}