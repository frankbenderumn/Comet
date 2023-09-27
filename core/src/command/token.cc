#include "comet/commands.h"

#include <unordered_map>

void update_token(const char* key, const char* val) {

    char* comet_dir = getenv("COMET_DIR");

    if (!comet_dir) {
        BRED("Comet directory env var does not exist!\n");
    }

    std::string token_path = std::string(comet_dir) +
                            "/.internal/config/tokens";
    const char* TOKEN_PATH = token_path.c_str();

    if (TOKEN_PATH == NULL) {
        printf("\033[1;31mCritical:\033[0m Token config file not assigned. " \
            "Program corrupted!\n");
        return;
    }

    if (access(TOKEN_PATH, F_OK) != -1) {

        // Create a temporary file to write the updated contents
        std::string temp_file_s = token_path + "_temp";
        
        FILE* fp = fopen(temp_file_s.c_str(), "wb");

        if (!fp) {
            BRED("Failed to open file '%s' for reading\n", temp_file_s.c_str());
            return;
        }

        std::ifstream file(TOKEN_PATH);

        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                size_t p;
                if ((p = line.find("export ")) != std::string::npos) {
                    std::string kv = line.substr(7);
                    size_t p2 = kv.find("=");
                    std::string skey = kv.substr(0, p2);
                    if (strcmp(skey.c_str(), key) == 0) {
                        std::string new_val(val);
                        std::string to_write = "export " +
                            skey + "=" + new_val + "\n";
                        fwrite(to_write.data(), 1, to_write.size(), fp);
                    } else {
                        line += "\n";
                        fwrite(line.data(), 1, line.size(), fp);
                    }
                }
            }
            file.close();
        } else {
            BRED("Failed to open file '%s'\n", token_path.c_str());
            fclose(fp);
            return; // Exit with an error code
        }

        fclose(fp);

        // Replace the original file with the updated file
        if (remove(TOKEN_PATH) != 0) {
            BRED("token-update: Failed to remove original file.\n");
            return;
        }
        if (rename(temp_file_s.c_str(), TOKEN_PATH) != 0) {
            BRED("token-update: Failed to rename temporary file.\n");
            return;
        }

    } else {
        BRED("Token file has not been initiated!\n");
        return;
    }

}

void token(const char* key, const char* val) {

    char* comet_dir = getenv("COMET_DIR");

    if (!comet_dir) {
        BRED("Comet directory env var does not exist!\n");
        return;
    }

    std::string token_path = std::string(comet_dir) +
                            "/.internal/config/tokens";
    
    std::vector<std::string> lines;
    std::ifstream file(token_path);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    } else {
        BRED("Failed to open file '%s'\n", token_path.c_str());
        return; // Exit with an error code
    }

    std::unordered_map<std::string, std::string> kvs;
    for (const auto& line : lines) {
        size_t p = line.find("export ");
        std::string l, skey, val;
        if (p != std::string::npos) {
            l = line.substr(7);
            size_t p2;
            if ((p2 = l.find("=")) != std::string::npos) {
                skey = l.substr(0, p2);
                val = l.substr(p2+1, l.size() - p2 - 1);
                kvs[skey] = val;
            }
        }
    }

    std::string skey(key);
    bool found = false;
    for (const auto& k : kvs) {
        if (skey == k.first) {
            found = true;
            break;
        }
    }

    if (found) {
        BRED("Token '%s' already exists, " \
            "please use token-update instead\n", skey.c_str());
        return;
    }

    FILE* fp = fopen(token_path.c_str(), "ab");
    if (!fp) {
        BRED("Failed to open file '%s'\n", token_path.c_str());
        return;
    }

    std::string toWrite = "export " + std::string(key) + "=" + std::string(val)
                            + "\n";

    fwrite(toWrite.data(), 1, toWrite.size(), fp);

    fclose(fp);

}

void token_load(const char* tokens_path) {
    char* comet_dir = getenv("COMET_DIR");

    if (!comet_dir) {
        BRED("Comet directory env var does not exist!\n");
        return;
    }

    std::string token_path;
    if (tokens_path == NULL) {
        token_path = "./comet.tokens";
    } else {
        token_path = std::string(tokens_path);        
    }
    
    std::vector<std::string> lines;
    std::ifstream file(token_path);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
            size_t p;
            std::string key, val;
            if ((p = line.find("=")) != std::string::npos) {
                key = line.substr(0, p);
                val = line.substr(p+1, line.size() - p - 1);
                token(key.c_str(), val.c_str());
            }
        }
        file.close();
    } else {
        BRED("Failed to open file '%s'\n", token_path.c_str());
        return; // Exit with an error code
    }
}