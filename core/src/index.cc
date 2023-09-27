#include "comet/index.h"

void CometIndex::update(Package* p) {
    char** files = (char**)malloc(sizeof(char*));
    int file_ct = 0;
    char* comet_dir = getenv("COMET_DIR");
    std::string pname = p->name();
    std::string registry = std::string(comet_dir) +
                        "/.internal/registry/" + pname + "/";
    if (!comet_dir) {
        BRED("CometIndex::update: Failed to get COMET_DIR env var!\n");
        return;
    }
    DLOG("Comet Dir: %s\n", registry.c_str());
    if (list_files(&files, (char*)registry.c_str(), &file_ct) < 0) {
        BRED("CometIndex::Update: Failed to list files for directory\n");
    }
    for (int i = 0; i < file_ct; i++) {
        free(files[i]);
    }
    free(files);
    if (exists(registry.c_str())) {
        remove_dir_r(registry.c_str());
    }
    int status = comet_pipe(p, "get");
    status = comet_pipe(p, "install");
    InstalledPackage* ip = get_by_name(pname);
    delete ip->attrs["updated_at"];
    delete ip->attrs["latest_str"];
    ip->attrs.erase("latest_str");
    ip->attrs.erase("updated_at");
    time_t ts = time(NULL);
    ip->attrs["updated_at"] = new CometAttr(
        new time_t(ts), COMET_TIMESTAMP, sizeof(time_t)
    );
    ip->attrs["latest_str"] = new CometAttr(
        new std::string(p->latest()), COMET_STR, 64
    );

    std::string temp = _index_path + "_temp";
    FILE* read = fopen(_index_path.c_str(), "rb");

    if (!read) {
        BRED("CometIndex::update: Failed to read file '%s'\n",
            _index_path.c_str());
        return;
    }

    FILE* write = fopen(temp.c_str(), "wb");

    if (!write) {
        BRED("CometIndex::update: Failed to create file '%s'\n",
            temp.c_str());
        return;
    }

    size_t header = 0;
    fread(&header, sizeof(size_t), 1, read);
    fwrite(&header, sizeof(size_t), 1, write);
    unsigned char buffer[header];
    int j = 0;
    fseek(read, 8, SEEK_SET);
    while (fread(buffer, 1, header, read) == header) {
        if (j != ip->row_id) {
            fwrite(buffer, 1, header, write);
        } else {
            std::string serialized = ip->serialize();
            fwrite(serialized.data(), 1, serialized.size(), write);
        }
        j++;
    }
    fclose(read);
    fclose(write);
    fremove(_index_path.c_str());
    if (rename(temp.c_str(), _index_path.c_str()) < 0) {
        BRED("Failed to rename file '%s' to '%s'\n", temp.c_str(), 
            _index_path.c_str());
    } 
}

void CometIndex::load_env() {
    char* comet_dir = getenv("COMET_DIR");
    if (!comet_dir) {
        BRED("Index load: Comet directory not found!\n");
        return;
    }

    // list files/dirs in COMET_DIR path
    int file_ct = 0;
    char** env_files = (char**)malloc(sizeof(char*));
    list_files(&env_files, comet_dir, &file_ct);

    // parses out the environments in the COMET_DIR
    std::vector<std::string> envs;
    for (int i = 0; i < file_ct; i++) {
        std::string prefix = std::string(comet_dir) + "/";
        char* end = remove_common_prefix(env_files[i], prefix.c_str());
        if (strcmp(end, ".internal") != 0 && strcmp(end, "envfile") != 0) {
            envs.push_back(std::string(end));
        }
        free(end);
    }
    this->envs = envs;

    // reads the envfile to get active/currently used environment
    // remember to remove \n at the end of name
    std::string envfile_path = std::string(comet_dir) + "/envfile";
    std::string content = fread(envfile_path.c_str());
    content.pop_back();
    this->env = content;

    for (int i = 0; i < file_ct; i++) free(env_files[i]);
    free(env_files);
}

int CometIndex::load(const std::string& index_path) {
    DLOGC("yel", "CometIndex::load: start\n");

    char* comet_dir = getenv("COMET_DIR");
    if (!comet_dir) {
        BRED("Index load: Comet directory not found!\n");
        return -1;
    }

    // list files/dirs in COMET_DIR path
    int file_ct = 0;
    char** env_files = (char**)malloc(sizeof(char*));
    list_files(&env_files, comet_dir, &file_ct);

    // parses out the environments in the COMET_DIR
    std::vector<std::string> envs;
    for (int i = 0; i < file_ct; i++) {
        std::string prefix = std::string(comet_dir) + "/";
        char* end = remove_common_prefix(env_files[i], prefix.c_str());
        if (strcmp(end, ".internal") != 0 && strcmp(end, "envfile") != 0) {
            envs.push_back(std::string(end));
        }
        free(end);
    }
    this->envs = envs;

    // reads the envfile to get active/currently used environment
    // remember to remove \n at the end of name
    std::string envfile_path = std::string(comet_dir) + "/envfile";
    std::string content = fread(envfile_path.c_str());
    content.pop_back();
    this->env = content;

    for (int i = 0; i < file_ct; i++) free(env_files[i]);
    free(env_files);

    // read index file in
    FILE* fp = fopen(index_path.c_str(), "rb");
    if (!fp) {
        DLOGC("red", "CometIndex::load: Failed to open file '%s'\n",
            index_path.c_str());
        return 1;
    }

    size_t header = 0;
    fread(&header, sizeof(size_t), 1, fp);

    DLOG("CometIndex::load: (Header) Record length: %li\n", header);

    unsigned char buffer[header];
    int parsed_row_length = header;
    this->row_length = header;
    int sz = fsize(fp);

    DLOG("CometIndex::load: File size: %i\n", sz);
    DLOG("CometIndex::load: Num records: %i\n",
        (int)((sz - sizeof(size_t)) / header));

    // if index file less than 16 in size data is likely corrupted
    if (sz < 16) {
        BRED("Corrupted Index file!\n");
        fclose(fp);
        return 0;
    }

    // read each row (using fixed header) and deserialize
    int j = 0;
    fseek(fp, 8, SEEK_SET);
    while (fread(buffer, 1, parsed_row_length, fp) == parsed_row_length) {
        InstalledPackage* ip = InstalledPackage::deserialize(
            buffer, parsed_row_length, j
        );
        installed.push_back(ip);
        j++;
    }

    fclose(fp);

    // return number of packages installed
    return j;

}

void CometIndex::list() {
    if (installed.size() > 0) {
        printf("Packages\n");
        printf("==================================================\n");
        for (const auto& i : installed) {
            i->show();
        }
    } else {
        RED("No packages installed!\n");
    }
}

InstalledPackage* CometIndex::get_by_name(const std::string& name) {
    for (const auto& i : installed) {
        std::string dis = i->name();
        dis.resize(name.size()); // not the most clean solution
        if (dis == name) {
            return i;
        }
    }
    return nullptr;
}

void CometIndex::remove(const std::string& name) {
    InstalledPackage* ip = get_by_name(name);
    if (!ip) {
        BRED("Comet::remove: Package '%s' not found in index!\n", name.c_str());
        return;
    }

    if (installed.size() == 1) {
        fremove(_index_path.c_str());
        return;
    }

    this->destroy_ledger(name);
    
    int row_to_remove = ip->row_id;
    std::string temp_path = _index_path + "_temp";
    FILE* read = fopen(_index_path.c_str(), "rb");
    FILE* write = fopen(temp_path.c_str(), "wb");
    if (!read) {
        BRED("Comet::remove: Failed to read index file!\n");
        return;
    }
    if (!write) {
        BRED("Comet::remove: Failed to create temp index file!\n");
        return;
    }
    int j = 0;
    size_t header = 0;
    fread(&header, sizeof(size_t), 1, read);
    size_t parsed_row_length = header;
    unsigned char buffer[header]; // should be dynamic not static
    fwrite(&header, sizeof(size_t), 1, write);
    fseek(read, 8, SEEK_SET);
    while (fread(buffer, 1, parsed_row_length, read) == parsed_row_length) {
        if (j != row_to_remove) {
            fwrite(buffer, 1, parsed_row_length, write);
        }
        j++;
    }
    fclose(write);
    fclose(read);
    fremove(_index_path.c_str());
    rename(temp_path.c_str(), _index_path.c_str());
}

void CometIndex::purge(const char* comet_dir, const std::string& name) {
    // need to delete cache install files
    this->remove(name);

    int file_ct = 0;
    char** files = (char**)malloc(sizeof(char*));

    std::string dir = std::string(comet_dir) + "/.internal/registry/" + name;
    list_files(&files, (char*)dir.c_str(), &file_ct);

    std::vector<std::string> ledgers;
    for (int i = 0; i < file_ct; ++i) {
        std::string l(files[i]);
        if (l.find("ledger-") != std::string::npos) {
            ledgers.push_back(l);
        }
    }

    if (ledgers.size() > 1) {
        for (const auto& l : ledgers) {
            std::string to_find = "ledger-" + this->env;
            if (l.find(to_find) != std::string::npos) {
                fremove(l.c_str());
            }
        }
    } else {
        remove_dir_r(dir.c_str());
    }

    for (int i = 0; i < file_ct; ++i) free(files[i]);
    free(files);

}

void CometIndex::add(InstalledPackage* ip) {

    this->create_ledger(ip);

    InstalledPackage* ipv = nullptr;

    if (!exists(_index_path.c_str())) {
        std::string serialized = ip->serialize();
        DLOG("CometIndex::add: Serialized size: %li == %li\n",
            serialized.size(), ip->size);
        FILE* fp = fopen(_index_path.c_str(), "wb");
        if (!fp) {
            BRED("CometIndex::add: Failed to create index file '%s'\n",
                _index_path.c_str());
            delete ip;
            return;
        }

        // Write the size of the serialized data record (fixed)
        size_t sz = serialized.size();
        if (fwrite(&sz, sizeof(size_t), 1, fp) != 1) {
            BRED("CometIndex::add: Failed to write serialized data size\n");
            fclose(fp);
            delete ip;
            return;
        }

        // Write the serialized data
        if (fwrite(serialized.data(), 1, serialized.size(), fp) !=
            serialized.size()
        ) {
            BRED("CometIndex::add: Failed to write serialized data\n");
            fclose(fp);
            delete ip;
            return;
        }

        fclose(fp);
        installed.push_back(ip);

    } else {
        std::string name = ip->name();
        ipv = this->get_by_name(name);
        if (ipv) {
            BRED("CometIndex::Add: Package '%s' already installed\n",
                name.c_str());
            delete ip;
            return;
        }

        FILE* fp = fopen(_index_path.c_str(), "ab");
        if (!fp) {
            BRED("CometIndex::add: Failed to create index file '%s'\n",
                _index_path.c_str());
            delete ip;
            return;
        }

        std::string serialized = ip->serialize();
        if (fwrite(serialized.data(), 1, serialized.size(), fp) !=
            serialized.size()
        ) {
            BRED("CometIndex::add: Failed to write serialized data\n");
            fclose(fp);
            delete ip;
            return;
        }

        fclose(fp);

        installed.push_back(ip);
    }
}

void CometIndex::destroy_ledger(const std::string& name) {
    char* comet_d = getenv("COMET_DIR");
    
    if (!comet_d) {
        BRED("remove: Fatal error: COMET_DIR not detected!\n");
        return;
    }
    
    std::string comet_dir(comet_d);
    comet_dir += "/.internal/registry/" + name + "/ledger-" + this->env;
    std::string prefix = std::string(comet_d) + "/.internal/registry/" +
                        name + "/build/";

    std::ifstream ledger(comet_dir);

    if (!ledger.is_open()) {
        std::cerr << "Ledger file does not exist: " << comet_dir << std::endl;
        return;
    }

    // Vector to store lines from the file
    std::vector<std::string> lines;

    std::string line;
    while (std::getline(ledger, line)) {
        lines.push_back(line);
    }

    // Close the file
    ledger.close();

    for (const auto& l : lines) {
        if (exists(l.c_str())) {
            if (path_type(l.c_str()) == PATH_DIR) {
                remove_dir_r(l.c_str());
            } else if (path_type(l.c_str()) == PATH_FILE) {
                fremove(l.c_str());
            }
        }
    }
}

void CometIndex::create_ledger(InstalledPackage* ip) {

    if (ip->source() != "apt-get") {

        // get installed files in registry
        char* dir = getenv("COMET_DIR");
        std::string path = std::string(dir) + "/.internal/registry/" +
                            ip->name();

        std::string ledger_path = path + "/ledger-" + this->env;
        std::string pack_path = path + "/build";
        std::string env_path = std::string(dir) + "/" + this->env;
        
        FILE* fp = fopen(ledger_path.c_str(), "wb");

        if (!fp) {
            BRED("Failed to create ledger file '%s'\n", ledger_path.c_str());
            delete ip;
            return;
        }

        int file_ct = 0;
        char** files = (char**)malloc(sizeof(char*));
        list_files(&files, (char*)pack_path.c_str(), &file_ct);

        for (int i = 0; i < file_ct; i++) {
            DLOG("CometIndex::create_ledger: FILE: %s\n", files[i]);
            DLOG("CometIndex::create_ledger: ENV: %s\n", env_path.c_str());
            char** subdir = (char**)malloc(sizeof(char*));
            int sub_ct = 0;
            list_files(&subdir, files[i], &sub_ct);
            for (int j = 0; j < sub_ct; j++) {
                std::string st = pack_path + "/";
                char* diff = remove_common_prefix(subdir[j], st.c_str());
                DLOGC("yel", "CometIndex::create_ledger: parsed_path: %s\n",
                    diff);
                std::string new_str = env_path + "/" + std::string(diff) + "\n";
                fwrite(new_str.data(), 1, new_str.size(), fp);
                free(diff);
                free(subdir[j]);
            }
            free(subdir);
            free(files[i]);
        }

        free(files);

        fclose(fp);

    }
}

/** TODO: this function may be redundant */
void CometIndex::install(Package* p) {
    InstalledPackage* ip = p->install();
    add(ip);
}

CometIndex::~CometIndex() {
    for (const auto& i : installed) {
        delete i;
    }
}

CometIndex::CometIndex(const std::string& comet_dir) {
    std::string env_path = comet_dir + "/envfile";
    load_env();
    _index_path = comet_dir + "/" + this->env + "/index";
    int status = load(_index_path);
}