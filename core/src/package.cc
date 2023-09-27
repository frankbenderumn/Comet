#include "comet/package.h"

CometAttr::~CometAttr() {
    if (type == COMET_STR) {
        delete static_cast<std::string*>(addr);
    } else if (type == COMET_TIMESTAMP) {
        delete static_cast<time_t*>(addr);
    }
}

Package::Package(const std::string& name,
        const std::string& source,
        const std::string& repo,
        const std::string& uri,
        const std::string& latest_str) {
    attrs["name"] = new CometAttr(new std::string(name), COMET_STR, 64);
    attrs["source"] = new CometAttr(new std::string(source), COMET_STR, 64);
    attrs["repo"] = new CometAttr(new std::string(repo), COMET_STR, 64);
    attrs["uri"] = new CometAttr(new std::string(uri), COMET_STR, 64);
    attrs["latest_str"] = new CometAttr(
        new std::string(latest_str), COMET_STR, 64
    );
    insertion_order = {"name", "source", "repo", "uri", "latest_str"};
    version = new SemVer(latest_str);
}

Package::~Package() {
    for (auto attr : attrs) {
        delete attr.second;
    }
    if (version) delete version;
}

void Package::dump() {
    printf("name: %s\n", (*(std::string*)attrs["name"]->addr).c_str());
    printf("source: %s\n", (*(std::string*)attrs["source"]->addr).c_str());
    printf("repo: %s\n", (*(std::string*)attrs["repo"]->addr).c_str());
    printf("uri: %s\n", 
        static_cast<std::string*>(attrs["uri"]->addr)->c_str());
    printf("latest_str: %s\n",
        (*(std::string*)attrs["latest_str"]->addr).c_str());
}

const std::string Package::name() {
    std::string result = "(null)";
    result = *(std::string*)attrs["name"]->addr;
    return result;
}

const std::string Package::latest() {
    return *(std::string*)attrs["latest_str"]->addr;
}

const std::string Package::source() {
    std::string result = "(null)";
    result = *(std::string*)this->attrs["source"]->addr;
    return result;
}

const std::string Package::serialize() const {
    return "base";
}

InstalledPackage* Package::install() {
    return new InstalledPackage(this);
}

InstalledPackage::InstalledPackage(Package* package) {
    this->_package = package;
    time_t timestamp = time(NULL);
    this->attrs["created_at"] = new CometAttr(
        new time_t(timestamp), COMET_TIMESTAMP, sizeof(time_t));
    this->attrs["updated_at"] = new CometAttr(
        new time_t(timestamp), COMET_TIMESTAMP, sizeof(time_t));
    for (const auto& attr : package->insertion_order) {
        this->size += package->attrs[attr]->size;
    }
    this->size += sizeof(time_t);
    this->size += sizeof(time_t);
}

InstalledPackage::InstalledPackage(const std::string& name,
                const std::string& source,
                const std::string& repo,
                const std::string& uri,
                const std::string& latest_str,
                const time_t& created_at,
                const time_t& updated_at,
                const int& row_id
            ) {
    this->attrs["name"] = new CometAttr(new std::string(name), COMET_STR, 64);
    this->attrs["source"] = new CometAttr(
        new std::string(source), COMET_STR, 64);
    this->attrs["repo"] = new CometAttr(new std::string(repo), COMET_STR, 64);
    this->attrs["uri"] = new CometAttr(new std::string(uri), COMET_STR, 64);
    this->attrs["latest_str"] = new CometAttr(
        new std::string(latest_str), COMET_STR, 64);
    this->attrs["created_at"] = new CometAttr(
        new time_t(created_at), COMET_TIMESTAMP, sizeof(time_t));
    this->attrs["updated_at"] = new CometAttr(
        new time_t(updated_at), COMET_TIMESTAMP, sizeof(time_t));
    this->row_id = row_id;
    version = new SemVer(latest_str);
}

InstalledPackage::~InstalledPackage() {
    for (auto m : attrs) {
        delete m.second;
    }
    if (version) delete version;
}

const std::string InstalledPackage::name() {
    std::string result = "(null)";
    if (_package != nullptr) {
        result = *(std::string*)_package->attrs["name"]->addr;
    } else {
        result = *(std::string*)this->attrs["name"]->addr;
    }
    return result;
}

const std::string InstalledPackage::source() {
    std::string result = "(null)";
    if (_package != nullptr) {
        result = *(std::string*)_package->attrs["source"]->addr;
    } else {
        result = *(std::string*)this->attrs["source"]->addr;
    }
    return result;
}

void InstalledPackage::dump() {
    printf("name: %s\n", 
        (*(std::string*)_package->attrs["name"]->addr).c_str());
    printf("source: %s\n", 
        (*(std::string*)_package->attrs["source"]->addr).c_str());
    printf("repo: %s\n", 
        (*(std::string*)_package->attrs["repo"]->addr).c_str());
    printf("uri: %s\n", (*(std::string*)_package->attrs["uri"]->addr).c_str());
    printf("latest_str: %s\n", 
        (*(std::string*)_package->attrs["latest_str"]->addr).c_str());
    printf("created_at: %li\n", (*(time_t*)attrs["created_at"]->addr));
    printf("updated_at: %li\n", (*(time_t*)attrs["updated_at"]->addr));
}

void InstalledPackage::show() {
    size_t max_length = 32;
    char created_at_buf[32];
    char updated_at_buf[32];
    printf("%-16s: %i\n", "row id", this->row_id);
    printf("%-16s: %s\n", "name",
        (*(std::string*)attrs["name"]->addr).c_str());
    printf("%-16s: %s\n", "source",
        (*(std::string*)attrs["source"]->addr).c_str());
    printf("%-16s: %s\n", "repo", (*(std::string*)attrs["repo"]->addr).c_str());
    printf("%-16s: %s\n", "uri", (*(std::string*)attrs["uri"]->addr).c_str());
    printf("%-16s: %s\n", "latest_str",
        (*(std::string*)attrs["latest_str"]->addr).c_str());
    time_t created_at = *(time_t*)attrs["created_at"]->addr;
    time_t updated_at = *(time_t*)attrs["updated_at"]->addr;
    struct tm* timeinfo = localtime(&created_at);
    strftime(
        created_at_buf, max_length, "%a, %b %d, %Y @ %I:%M:%S %p", timeinfo);
    printf("%-16s: %s\n", "created at", created_at_buf);
    struct tm* timeinfo2 = localtime(&updated_at);
    strftime(
        updated_at_buf, max_length, "%a, %b %d, %Y @ %I:%M:%S %p", timeinfo2);
    printf("%-16s: %s\n", "updated at", updated_at_buf);
    printf("-------------------------------------------------------------\n");
}

const std::string InstalledPackage::serialize() {
    std::ostringstream oss;
    size_t offset = 0;

    if (_package) {
        // may break if not strings implemented
        for (const auto& attr : _package->insertion_order) {
            std::string s = *(std::string*)_package->attrs[attr]->addr;
            size_t size = _package->attrs[attr]->size;
            s.resize(size);
            offset += size;
            oss.write(s.data(), size);
        }

        std::vector<std::string> insertion_order = {"created_at", "updated_at"};

        for (const auto& attr : insertion_order) {
            if (attrs[attr]->type == COMET_TIMESTAMP) {
                time_t ts = *static_cast<time_t*>(attrs[attr]->addr);
                oss.write(
                    reinterpret_cast<const char*>(&ts), attrs[attr]->size);
                offset += size;
            } else if (attrs[attr]->type == COMET_STR) {
                std::string s = *(std::string*)attrs[attr]->addr;
                size_t size = attrs[attr]->size;
                s.resize(size);
                offset += size;
                oss.write(s.data(), size);
            }
        }

    } else {

        std::vector<std::string> insertion_order = {"name", "source", "repo",
            "uri", "latest_str", "created_at", "updated_at"};

        for (const auto& attr : insertion_order) {
            if (attrs[attr]->type == COMET_TIMESTAMP) {
                time_t ts = *static_cast<time_t*>(attrs[attr]->addr);
                oss.write(
                    reinterpret_cast<const char*>(&ts), attrs[attr]->size);
                offset += size;
            } else if (attrs[attr]->type == COMET_STR) {
                std::string s = *(std::string*)attrs[attr]->addr;
                size_t size = attrs[attr]->size;
                s.resize(size);
                offset += size;
                oss.write(s.data(), size);
            }
        }

    }

    DLOG("InstalledPackage::serialize: offset is: %li\n", offset);

    return oss.str();
}

InstalledPackage* InstalledPackage::deserialize(const unsigned char* buffer,
                                size_t record_length, int row_id) {
    DLOGC("yel", "InstalledPackage::deserialize: start\n");
    std::string buf(reinterpret_cast<const char*>(buffer), record_length);
    std::string name = buf.substr(0, 64);
    std::string source = buf.substr(64, 64);
    std::string repo = buf.substr(128, 64);
    std::string uri = buf.substr(192, 64);
    std::string latest_str = buf.substr(256, 64);

    // We'll store the timestamp as a 64-bit unsigned integer
    uint64_t timestamp;
    uint64_t timestamp2;

    // Copy the 8 bytes of binary data into 'timestamp'
    std::memcpy(&timestamp, buffer + 320, sizeof(uint64_t));
    std::memcpy(&timestamp2, buffer + 328, sizeof(uint64_t));
    
    // Convert to a time_t (Unix timestamp is typically a 32-bit or
    // 64-bit integer)
    time_t created_at = static_cast<time_t>(timestamp);
    time_t updated_at = static_cast<time_t>(timestamp2);

    InstalledPackage* ip = new InstalledPackage(
        name, source, repo, uri, latest_str, created_at, updated_at, row_id);
    return ip;
}