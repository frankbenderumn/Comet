#include "comet/color.h"
#include "comet/comet.h"

int main(int argc, char* argv[]) {

    const char* TOKEN_PATH = "./config/tokens";
    const char* COMET_DIR = "./local-test-1";

    // fixtures
    const char* command_1 = "command_1";
    const char* option_1 = "option_1";

    if (COMET_DIR == NULL) {
        BRED("Comet config file not found!\n");
        return -1;
    }

    // if (argc < 2) {
    //     printf("\033[1;31mComet: Invalid number of arguments! Please use comet --help for more options!\033[0m\n");
    //     return -1;
    // }

    char* COMMAND = (char*)command_1;
    YEL("Command: %s\n", COMMAND);
    char* OPTION = NULL; 

    // if (argc > 2) {
        OPTION = (char*)option_1;
    // }

    // int ROW_LENGTH = 136;
    int ROW_LENGTH = 126;

    char INDEX_PATH[strlen(COMET_DIR) + strlen("/.internal/index") + 1];
    memcpy(INDEX_PATH, COMET_DIR, strlen(COMET_DIR));
    memcpy(INDEX_PATH + strlen("/.internal/index"), "/.internal/index", strlen("/.internal/index"));
    INDEX_PATH[strlen(COMET_DIR) + strlen("/.internal/index")] = 0;

    Package* packages = NULL;

    if (comet_setup(COMET_DIR) < 0) {
        BRED("Corrupted file structure on comet!\n");
        return -1;
    }

    if (strcmp(COMMAND, "init") != 0) {
        if (verify_dirs() < 0) {
            BRED("Comet has not been initialized, please run 'sudo comet init'\n");
            return -1;
        }
    }

    std::string dir_1 = "./local-test-1/package";
    // std::string dir_1 = "./local-test-1/package";
    // std::string dir_1 = "./local-test-1/package";    
    // std::string dir_1 = "./local-test-1/package";

    return 1;
}