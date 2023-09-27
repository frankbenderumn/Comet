#include "comet/manifest.h"

int CometManifest::load() {
    DLOGC("yel", "CometManifest::load: start\n");
    int manifest_size = 0;

    // checks if manifest path exists ands if so parse the json into iterable
    // format
    if (exists(_manifest_path.c_str())) {
        picojson::value data;

        std::ifstream nodes(_manifest_path);
        std::stringstream buf;
        buf << nodes.rdbuf();
        std::string json = buf.str();
        std::string err = picojson::parse(data, json);
        if (!err.empty()) {
            std::cerr << err << std::endl;
            BRED("Failed to parse json file: %s\n", _manifest_path.c_str());
            return -1;
        }

        // uses series of format checks to ensure consistency
        if (!data.is<picojson::array>()) {
            BRED("Json file '%s' not a json array!\n", _manifest_path.c_str());
            return -1;
        }

        picojson::array arr = data.get<picojson::array>();

        DLOGC("yel", "CometManifest::Load: Number of package manifests: %li\n",
            arr.size());

        // loops through each package in the parsed JSON manifest
        for (int i = 0; i < arr.size(); i++) {
            picojson::object o = arr[i].get<picojson::object>();
            picojson::array versions_arr; // keeps track of various versions
            picojson::array deps_arr; // stores names of package dependencies
            int status = 0;
            std::string name;
            std::string latest_str; // string version of the most recent version
            std::string uri;
            std::string source;
            std::string repo;
            std::vector<std::string> deps;
            std::vector<std::string> versions;

            // existence checks. these five keys are mandatory in manifest
            // (name, uri, repo, source, latest)
            if (o.find("name") != o.end()) {
                name = o["name"].get<std::string>();
            } else {
                RED("Package Manifest: No name detected!\n");
                return -1;
            }

            if (o.find("uri") != o.end()) {
                uri = o["uri"].get<std::string>();
            } else {
                RED("Package Manifest: No uri detected!\n");
                return -1;
            }

            if (o.find("repo") != o.end()) {
                repo = o["repo"].get<std::string>();
            } else {
                RED("Package Manifest: No repo detected!\n");
                return -1;
            }

            if (o.find("source") != o.end()) {
                source = o["source"].get<std::string>();
            } else {
                RED("Package Manifest: No source detected!\n");
                return -1;
            }

            if (o.find("latest") != o.end()) {
                latest_str = o["latest"].get<std::string>(); 
            } else {
                RED("Package Manifest: No recent version detected!\n");
                return -1;
            }

            // deps and versions are optional and array format
            if (o.find("deps") != o.end()) {
                if (o["deps"].is<picojson::array>()) {
                    for (int i = 0; i < o["deps"].get<picojson::array>().size();
                      i++) {
                        deps.push_back(o["deps"].get<picojson::array>()[i].get<std::string>());
                    }
                }
            }

            if (o.find("versions") != o.end()) {
                if (!o["versions"].is<picojson::array>()) {
                    for (int i = 0; i < o["versions"].get<picojson::array>().size();
                      i++) {
                        versions.push_back(o["versions"].get<picojson::array>()[i].get<std::string>());                        
                    }
                }
            }

            // not enforced currently, but used for determining max length of 
            // string vars. 64 is used for all except uri.
            if (name.size() > 31) {
                RED("Package Manifest name is too long. MAX 31 characters!\n");
                return -1;
            }

            if (source.size() > 31) { 
                RED("Package Manifest source is too long. MAX 31 characters!\n");
                return -1;
            }

            if (repo.size() > 31) { 
                RED("Package Manifest repo is too long. MAX 31 characters!\n");
                return -1;
            }

            if (uri.size() > 127) {
                RED("Package Manifest uri is too long. MAX 127 characters!\n");
                return -1;
            }

            DLOG("CometManifest::load: name: %s\n", name.c_str());
            DLOG("CometManifest::load: source: %s\n", source.c_str());
            DLOG("CometManifest::load: repo: %s\n", repo.c_str());
            DLOG("CometManifest::load: uri: %s\n", uri.c_str());
            DLOG("CometManifest::load: latest: %s\n", latest_str.c_str());
            DLOG("-------------------------------------------------------\n");

            // creates package object with parsed information
            Package* p = new Package(name, source, repo, uri, latest_str);
            p->deps = deps;
            p->versions = versions;

            // a uri may have an environment variable in it.
            // i.e. https://user:${PASSWORD}@github.com
            // we never want to print this info or make it ready available 
            // so this code interpolates it and handles it in memory without ever
            // storing it in the binary file or persisting it
            std::string uri_interpolated = *static_cast<std::string*>(p->attrs["uri"]->addr);
            if (uri_env(uri_interpolated, (char*)name.c_str(), (char*)latest_str.c_str()) < 0) {
                printf("\033[1;31mError\033[0m CometManifest::load: Environment variable not found in\n\t%s!\n", uri.c_str());
                delete p;

            } else {
                // resize required to prevent buffer overflow with serialization
                uri_interpolated.resize(256);
                p->attrs["uri_interpolated"] = new CometAttr(new std::string(uri_interpolated), COMET_STR, 256);

                // keep track of all loaded manifests in memory
                _packages.push_back(p);
            }
        }

    } else {
        RED("Critical: No comet manifest detected!\n");
        return -1;
    }

    return manifest_size;
}

CometManifest::~CometManifest() {
    for (auto p : _packages) {
        delete p;
    }
}

Package* CometManifest::get_by_name(const std::string& name) {
    for (const auto& p : _packages) {
        if ((*(std::string*)p->attrs["name"]->addr) == name) return p;
    }
    return nullptr;
}

CometManifest::CometManifest(const std::string& manifest_path) {
    _manifest_path = manifest_path;
    // may be worth doing outside of constructor, as constructors can't
    // throw errors without exceptions, exit may be a vulnerability
    if (load() < 0) {
        BRED("Failed to load manifest!\n");
    }
}