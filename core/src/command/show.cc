#include "comet/commands.h"

void show(Comet* comet, const std::string& package_name) {
    InstalledPackage* ip = comet->index->get_by_name(package_name);
    if (ip) {
        ip->show();
    } else {
        BRED("Package '%s' not installed\n", package_name.c_str())
    }
}