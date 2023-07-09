#include "comet/command/update.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("\033[1;31mError:\033[0m No package provided to update!. Usage: comet update <package>\n");
        return 1;
    }

    update(argv[1]);

    return 0;
}