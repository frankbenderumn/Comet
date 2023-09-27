#include "comet/semver.h"

#include <vector>

SemVer::SemVer(const std::string& version) {
    this->string = version;
    char delimiter = '.';
    std::vector<std::string> components;
    std::istringstream stream(version);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        components.push_back(token);
    }

    size_t sz = components.size();
    if (sz == 3) {
        this->patch = std::stoi(components[2]);
        this->minor = std::stoi(components[1]);
        this->major = std::stoi(components[0]);
    } else if (sz == 2) {
        this->minor = std::stoi(components[1]);
        this->major = std::stoi(components[0]);
    } else if (sz == 0) {
        this->major = std::stoi(components[0]);
    } else {
        major = 0;
        minor = 0;
        patch = 0;
        string = "0.0.0";
    }
}

std::string SemVer::stringify() {
    std::string result;
    if (major != -1) { result += std::to_string(major); }
    if (minor != -1) { result += std::to_string(minor); }
    if (patch != -1) { result += std::to_string(patch); }
    return result;
}

int SemVer::compare(SemVer* ext) {
    if (this->major == -1 || ext->major == -1) {
        return -1;
    }

    if (this->major != ext->major) {
        if (this->major > ext->major) {
            return 1;
        } else if (this->major < ext->major) {
            return 2;
        }
    }

    if (this->minor != ext->minor) {
        if (this->minor > ext->minor) {
            return 1;
        } else if (this->minor < ext->minor) {
            return 2;
        }
    }

    if (this->patch != ext->patch) {
        if (this->patch > ext->patch) {
            return 1;
        } else if (this->patch < ext->patch) {
            return 2;
        }
    }

    return 0;
}