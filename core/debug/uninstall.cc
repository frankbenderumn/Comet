#include "comet/command/uninstall.h"
#include "comet/command/install.h"
#include "comet/command/get.h"
#include "comet/manifest.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("\033[1;31mError:\033[0m No package provided for uninstall command! Usage: comet uninstall <package>\n");
        return 1;
    }

    uninstall(argv[1]);

    BGRE("Gracefully terminated!\n");

    return 0;
}