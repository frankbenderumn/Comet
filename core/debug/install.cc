#include "comet/command/install.h"
#include "comet/command/get.h"
#include "comet/manifest.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("\033[1;31mError:\033[0m No package provided for install command! \033[1;36mUsage:\033[0m comet install <package>\n");
        return 1;
    }

    install(argv[1]);
}