#include "comet/comet_manager.h"

Comet::Comet(const std::string& index_path, const std::string& manifest_path) {
    _index_path = index_path;
    _manifest_path = manifest_path;
    manifest = new CometManifest(manifest_path);
    index = new CometIndex(index_path);
}

Comet::~Comet() {
    delete manifest;
    delete index;
}

void Comet::install(Package* package) {
    DLOGC("yel", "Comet::install: start\n");
    index->install(package);
}

void Comet::update(const std::string& package_name) {
    InstalledPackage* ip = index->get_by_name(package_name);
    if (!ip) {
        BRED("Comet: Can't update package '%s' because it is not installed!\n",
            package_name.c_str());
        return;
    }
    Package* p = manifest->get_by_name(package_name);
    for (const auto& dep : p->deps) {
        InstalledPackage* ip = index->get_by_name(dep);
        if (!ip) {
            Package* d = manifest->get_by_name(dep);
            if (d) {
                this->install(d);
            } else {
                BRED("Comet::update: The dep '%s' does not exist!\n",
                    dep.c_str());
            }
        }
    }
    int rel = p->version->compare(ip->version);
    DLOGC("blue", "Comet::update: VERSION COMPARISON: %i\n", rel);
    if (rel == 1) {
        index->update(p);
    }
}