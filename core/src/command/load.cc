#include "comet/commands.h"
#include "picojson.h"

#include <iostream>
#include <fstream>
#include <utility>
#include <cctype>

typedef std::tuple<std::string, std::string, std::string> VersionTuple;
VersionTuple _parse_version(const std::string& str) {
    std::string key, val, command;
    bool split = false;
    for (const auto& c : str) {
        if (c == '=' || c == '<' || c == '>') {
            split = true;
            command += c;
            continue;
        }
        if (!split) {
            key += c;
        } else {
            val += c;
        }
    }
    
    DLOG("key: %s\n", key.c_str());
    DLOG("val: %s\n", val.c_str());
    DLOG("command: %s\n", command.c_str());

    return {key, val, command};
}

// not fully implemented
void specific_install(Comet* comet,
                    const char* COMET_DIR,
                    const std::string& name,
                    const std::string& version,
                    const std::string& cmd
                ) {

    Package* manifest = comet->manifest->get_by_name(name);

    InstalledPackage* ip = comet->index->get_by_name(name);
    
    if (cmd == "==") {

    } else if (cmd == ">=") {

    } else if (cmd == ">") {

    }

    if (manifest) {
        if (!ip) {
            int status = comet_pipe(manifest, "get");
            status = comet_pipe(manifest, "install");
            printf("Install status: %i\n", status);
            comet->install(manifest);
        } else {
            BRED("Package '%s' already installed!\n", name.c_str());
        }
    } else {
        BRED("specific_install: Package '%s' not in manifest!\n", name.c_str());
        return;
    }

}

void load(Comet* comet, char* path) {

    char* comet_d = getenv("COMET_DIR");

    if (!comet_d) {
        BRED("Comet dir does not exist!\n");
        return;
    }

    if (path == NULL) {
        if (!exists("./comet.lock")) {
            BRED("COMET FILE does not exist!\n");
            return;
        }
        
        std::string default_path = "./comet.lock";
        path = (char*)default_path.c_str();

        if (!exists(default_path.c_str())) {
            BRED("No load file provided and default comet.lock " \
             "file does not exist!\n");
            return;
        }
    }

    picojson::value data;

    std::string s(path);

    std::ifstream nodes(s);

    if (!nodes.is_open()) {
        std::cerr << "File not found: " << path << std::endl;
        return;
    }

    std::stringstream buf;
    buf << nodes.rdbuf();
    std::string json = buf.str();
    std::cout << json << std::endl;
    std::string err = picojson::parse(data, json);
    if (!err.empty()) {
        std::cerr << err << std::endl;
        BRED("Failed to parse json file: %s\n", path);
        return;
    }

    if (data.is<picojson::object>()) {
        const picojson::object& obj = data.get<picojson::object>();

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            const std::string& key = it->first;
            const picojson::value& value = it->second;
            std::cout << "Value is: " << value << std::endl;
        
            if (!value.is<picojson::array>()) {
                BRED("Error: JSON content is not an array!\n");
                continue;
            } else if (value.is<std::string>()) {
                GRE("String detected!\n");
                continue;
            }

            picojson::array arr = value.get<picojson::array>();

            for (size_t i = 0; i < arr.size(); i++) {
                if (!arr[i].is<std::string>()) {
                    BRED("Comet array item is not a string!\n");
                }
                std::string pack = arr[i].get<std::string>();
                BLU("pack: %s\n", pack.c_str());
                VersionTuple pair = _parse_version(pack);
                std::string key = std::get<0>(pair);
                std::string val = std::get<1>(pair);
                std::string command = std::get<2>(pair);

                if (!command.empty()) {
                    specific_install(comet, comet_d, key, val, command);
                } else if (val.empty()) {
                    install(comet, key);
                    BLU("Basic...\n");
                }
            }

        }
    }


}