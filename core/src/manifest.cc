#include "comet/manifest.h"
#include "comet/setup.h"

int safe_atoi(const char* str, int* result) {
    if (str == NULL || result == NULL)
        return 0; // Invalid input parameters

    // Skip leading whitespace
    while (isspace((unsigned char)(*str)))
        str++;

    // Check for sign
    int sign = 1;
    if (*str == '-' || *str == '+') {
        if (*str == '-')
            sign = -1;
        str++;
    }

    // Convert digits to integer
    int value = 0;
    while (isdigit((unsigned char)(*str))) {
        // Check for potential overflow
        if (value > (INT_MAX / 10) || (value == (INT_MAX / 10) && (*str - '0') > (INT_MAX % 10))) {
            return 0; // Integer overflow occurred
        }

        value = (value * 10) + (*str - '0');
        str++;
    }

    *result = sign * value;
    return 1; // Successful conversion
}


int semver_construct(SemVer* sv, char* version) {
    char** ps = NULL;
    int i = 0;
    char* p = NULL;
    char* start = version;
    while((p = strchr(start, '.')) != NULL) {
        ps = (char**)realloc(ps, sizeof(char*) * (i + 1));
        ps[i] = strndup(start, p - start);
        start = p + 1;
        i++;
    }
    ps = (char**)realloc(ps, sizeof(char*) * (i+1));
    ps[i] = strdup(start);
    i++;
    sv->major = -1;
    sv->minor = -1;
    sv->patch = -1;
    if (i == 1) {
        if (strcmp(ps[0], "N/A") == 0) {
        } else {
            int m = safe_atoi(ps[0], &sv->major);
        }
    } else if (i == 2) {
        safe_atoi(ps[0], &sv->major);
    } else if (i == 3) {
        safe_atoi(ps[0], &sv->major);
        safe_atoi(ps[1], &sv->minor);
    } else if (i == 4) {
        safe_atoi(ps[0], &sv->major);
        safe_atoi(ps[1], &sv->minor);
        safe_atoi(ps[2], &sv->patch);
    } else {

        int j = 0;
        while (j < i) {
            if (ps[j]) free(ps[j]);
            j++;
        }

        free(ps);

        return -1;
    }
    strncpy(sv->string, version, strlen(version));
    sv->string[strlen(version)] = 0;

    int j = 0;
    while (j < i) {
        if (ps[j]) free(ps[j]);
        j++;
    }

    free(ps);

    return i;
}

int semver_compare(SemVer* a, SemVer* b) {
    if (a->major == -1 || b->major == -1) {
        return -1;
    }
    
    if (a->major != b->major) {
        if (a->major > b->major) {
            return 1;
        } else if (a->major < b->major) {
            return 2;
        }
    }

    if (a->minor != b->minor) {
        if (a->minor > b->minor) {
            return 1;
        } else if (a->minor < b->minor) {
            return 2;
        }
    }

    if (a->patch != b->patch) {
        if (a->patch > b->patch) {
            return 1;
        } else if (a->patch < b->patch) {
            return 2;
        }
    }


    return 0;
}

void manifest_dump(PackageManifest* pm) {
    BMAG("Package Manifest: %s\n", pm->name);
    MAG("Source: %s\n", pm->source);
    MAG("URI: %s\n", pm->uri);
    MAG("Repo: %s\n", pm->repo);
    MAG("Latest: %s\n", pm->latest->string);
    MAG("Deps: %i\n", pm->deps_sz);
    MAG("Versions: %i\n", pm->versions_sz);
    MAG("Need to iterate versions\n");
}

// may need to alter for version specification
int uri_env(char* uri, const char* name, const char* version) {
    regex_t regex;
    int ret;

    // Compile the regular expression
    ret = regcomp(&regex, "\\$\\{[^}]+\\}", REG_EXTENDED);
    if (ret) {
        printf("Failed to compile the regular expression.\n");
        return -1;
    }

    // Match the regular expression against the string
    regmatch_t match;
    char* mutable_uri = strdup(uri);  // Create a mutable copy of the URI string
    while (regexec(&regex, mutable_uri, 1, &match, 0) == 0) {
        // Get the matched substring
        char* matched_substring = strndup(&mutable_uri[match.rm_so], match.rm_eo - match.rm_so);
        // printf("Match: %s\n", matched_substring);

        char env_var[strlen(matched_substring) - 3 + 1];
        strncpy(env_var, matched_substring + 2, strlen(matched_substring) - 3);
        env_var[strlen(matched_substring) - 3] = 0;
        // YEL("ENV VAR: %s\n", env_var);

        // Perform the substitution
        // Replace the ${VERSION} tag with the desired value
        char* replacement = NULL;
        char* comet_dir = getenv("COMET_DIR");
        if (comet_dir == NULL) {
            BRED("No COMET_DIR environment\n");
            regfree(&regex);
            free(mutable_uri);
            free(matched_substring);
            return -1;
        }

        if (strcmp(env_var, "GET") == 0) {
            replacement = (char*)malloc(24 + strlen(comet_dir) + 1);
            strncpy(replacement, comet_dir, strlen(comet_dir));
            strncpy(replacement + strlen(comet_dir), "/.internal/manifest/.get", 24);
            replacement[24 + strlen(comet_dir)] = 0;
        } else if (strcmp(env_var, "VERSION") == 0) {
            replacement = strdup(version);
        } else {
            char* env_value = getenv(env_var);
            if (env_value == NULL) {
                printf("No replacement found for URI interpolation!\n");
                free(mutable_uri);
                free(matched_substring);
                regfree(&regex);
                return -1;
            }
            replacement = strdup(env_value);
        }

        if (replacement == NULL) {
            RED("No replacement found for uri interpolation!\n");
            regfree(&regex);
            free(mutable_uri);
            free(matched_substring);
            return -1;
        }

        // BLU("\n\nReplacement is %s\n", replacement);
        // YEL("Mutable uri is: %s\n", mutable_uri);
        // GRE("Start: %i, End: %i\n", match.rm_so, match.rm_eo);
        // GRE("Replacement length: %li\n", strlen(replacement));
        // GRE("Init length: %i\n", match.rm_so);
        // GRE("Remainder length: %li\n", strlen(mutable_uri) - match.rm_eo);
        // printf("\n\n");
        size_t new_uri_sz = strlen(replacement) + match.rm_so + strlen(mutable_uri) - match.rm_eo;
        char new_uri[new_uri_sz + 1];
        memcpy(new_uri, mutable_uri, (long)match.rm_so);
        memcpy(new_uri + match.rm_so, replacement, strlen(replacement));
        memcpy(new_uri + match.rm_so + strlen(replacement), mutable_uri + match.rm_eo, strlen(mutable_uri) - match.rm_eo);
        new_uri[new_uri_sz] = 0;
        // BWHI("New string: %s\n", new_uri);

        mutable_uri = (char*)realloc(mutable_uri, strlen(new_uri) + 1);
        strncpy(mutable_uri, new_uri, strlen(new_uri));
        mutable_uri[new_uri_sz] = 0;

        if (replacement) {
            free(replacement);
        }

        // Free the memory allocated for the matched substring
        free(matched_substring);
    }

    // printf("Updated URI: %s\n", mutable_uri);

    // Clean up the regular expression
    regfree(&regex);

    strncpy(uri, mutable_uri, strlen(mutable_uri));
    uri[strlen(mutable_uri)] = 0;

    // Free the memory allocated for the mutable URI
    free(mutable_uri);

    return 1;
}

void free_temp_arr(char** temp_arr, int size) {
    for (int i = 0; i < size; i++) {
        free(temp_arr[i]);
    }
    free(temp_arr);
}


void manifest_array_destruct(PackageManifest*** arr, int sz) {
    
    for (int i = 0; i < sz; i++) {
        if ((*arr)[i]->latest) free((*arr)[i]->latest);
        if ((*arr)[i]->deps_sz > 0) free_temp_arr((*arr)[i]->deps, (*arr)[i]->deps_sz);
        if ((*arr)[i]->versions_sz > 0) free_temp_arr((*arr)[i]->versions, (*arr)[i]->versions_sz);
        if ((*arr)[i]) free((*arr)[i]);
    }
    if (*arr) free(*arr);
}

PackageManifest* manifest_get_by_name(PackageManifest** manifest, int num, const char* name) {
    printf("\033[1;35mmanifest_get_by_name\033[0m: number of package manifests is %i\n", num);
    printf("\033[1;35mmanifest_get_by_name\033[0m: searching for package manifest '%s'\n", name);
    for (int i = 0; i < num; i++) {
        if (manifest[i]) {
            printf("\033[1;35mmanifest_get_by_name\033[0m: reading '%s'\n", manifest[i]->name);
            if (strcmp(manifest[i]->name, name) == 0) {
                GRE("manifest_get_by_name: found '%s'\n", name);
                return manifest[i];
            }
        } else {
            BRED("Package Manifest is NULL somehow\n");
        }
    }
    return NULL;
}

int pico_to_arr(picojson::object& o, const char* key, char*** arr, int* arr_size) {
    if (o.find(key) != o.end()) {
        if (!o[key].is<picojson::array>()) {
            BRED("%s key not of type array!\n", key);
            return -1;
        }
        picojson::array pico_arr = o[key].get<picojson::array>();

        if (pico_arr.size() == 0) { return -1; }

        *arr = (char**)malloc(sizeof(char*) * pico_arr.size());
        if (*arr == NULL) {
            BRED("Failed to allocate memory for temp_arr\n");
            return -1;
        }

        int i = 0;
        for (; i < pico_arr.size(); i++) {
            std::string el;
            if (!pico_arr[i].is<std::string>()) {
                BRED("Deps key vals are not of type string!\n");
                break;
            }
            el = pico_arr[i].get<std::string>();
            (*arr)[i] = (char*)malloc(el.size() + 1);
            if ((*arr)[i] == NULL) {
                BRED("Failed to allocate memory for temp_arr[%d]\n", i);
                break;
            }
            strncpy((*arr)[i], el.c_str(), el.size());
            (*arr)[i][el.size()] = '\0';
        }

        if (i < pico_arr.size()) {
            // Error occurred during allocation, free the allocated elements
            free_temp_arr(*arr, i);
            free(*arr);  // Free the array itself
            *arr = NULL; // Set the array pointer to NULL
            return -1;
        }

        *arr_size = pico_arr.size();
    }
    return 1;
}

int manifest_load(PackageManifest*** manifest, const char* comet_manifest) {
    int manifest_size = 0;
    if (exists(comet_manifest)) {
        picojson::value data;

        std::ifstream nodes(comet_manifest);
        std::stringstream buf;
        buf << nodes.rdbuf();
        std::string json = buf.str();
        // BCYA("Json is: %s\n", json.c_str());
        std::string err = picojson::parse(data, json);
        if (!err.empty()) {
            std::cerr << err << std::endl;
            BRED("Failed to parse json file: %s\n", comet_manifest);
            return -1;
        }

        if (!data.is<picojson::array>()) {
            BRED("Json file '%s' not a json array!\n", comet_manifest);
            return -1;
        }

        picojson::array arr = data.get<picojson::array>();


        for (int i = 0; i < arr.size(); i++) {
            picojson::object o = arr[i].get<picojson::object>();
            *manifest = (PackageManifest**)realloc(*manifest, sizeof(PackageManifest*) * (i + 1));
            // will need single line declarations in C
            std::string name_str;
            std::string uri_str;
            std::string source_str;
            std::string latest_str;
            std::string repo_str;
            picojson::array versions_arr;
            picojson::array deps_arr;
            SemVer* latest = NULL;
            char** versions = NULL;
            char** deps = NULL;
            int versions_sz = 0;
            int deps_sz = 0;
            int status = 0;

            if (o.find("name") != o.end()) {
                name_str = o["name"].get<std::string>();
            } else {
                RED("Package Manifest: No name detected!\n");
                manifest_array_destruct(manifest, i + 1);
                // exit(1);
                return -1;
            }

            if (o.find("latest") != o.end()) {
                latest_str = o["latest"].get<std::string>(); 
            } else {
                RED("Package Manifest: No recent version detected!\n");
                manifest_array_destruct(manifest, i + 1);
                // free((*manifest)[i]);
                return -1;
            }

            if (o.find("uri") != o.end()) {
                uri_str = o["uri"].get<std::string>();
            } else {
                RED("Package Manifest: No uri detected!\n");
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            if (o.find("repo") != o.end()) {
                repo_str = o["repo"].get<std::string>();
            }

            if (o.find("source") != o.end()) {
                source_str = o["source"].get<std::string>();
            } else {
                RED("Package Manifest: No source detected!\n");
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            char* name = strdup(name_str.c_str());
            char* uri = strdup(uri_str.c_str());
            char* source = strdup(source_str.c_str());
            char* repo = strdup(repo_str.c_str());
            latest = (SemVer*)calloc(1, sizeof(SemVer));
            status = semver_construct(latest, (char*)latest_str.c_str());

            if (status == -1) {
                RED("SemVer format incorrect!\n");
                free(latest);
                free(latest);
                free(name);
                free(source);
                free(repo);
                free(uri);
                // free_temp_arr(deps, deps_sz);
                // free_temp_arr(versions, versions_sz);
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            if (strlen(name) > 31) {
                RED("Package Manifest name is too long. MAX 31 characters!\n");
                free(latest);
                free(name);
                free(source);
                free(repo);
                free(uri);
                // free_temp_arr(deps, deps_sz);
                // free_temp_arr(versions, versions_sz);
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            if (strlen(source) > 31) { 
                RED("Package Manifest source is too long. MAX 31 characters!\n");
                free(latest);
                free(name);
                free(source);
                free(repo);
                free(uri);
                // free_temp_arr(deps, deps_sz);
                // free_temp_arr(versions, versions_sz);
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            if (strlen(repo) > 31) { 
                RED("Package Manifest repo is too long. MAX 31 characters!\n");
                free(latest);
                free(name);
                free(source);
                free(repo);
                free(uri);
                // free_temp_arr(deps, deps_sz);
                // free_temp_arr(versions, versions_sz);
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            if (strlen(uri) > 127) {
                RED("Package Manifest uri is too long. MAX 127 characters!\n");
                free(latest);
                free(name);
                free(source);
                free(repo);
                free(uri);
                // free_temp_arr(deps, deps_sz);
                // free_temp_arr(versions, versions_sz);
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            (*manifest)[i] = (PackageManifest*)calloc(1, sizeof(PackageManifest));
            (*manifest)[i]->latest = NULL;

            strncpy((*manifest)[i]->name, name, sizeof((*manifest)[i]->name) - 1);
            (*manifest)[i]->name[sizeof((*manifest)[i]->name) - 1] = '\0';
            (*manifest)[i]->deps = NULL;
            (*manifest)[i]->versions = NULL;
            (*manifest)[i]->deps_sz = 0;
            (*manifest)[i]->versions_sz = 0;

            // BYEL("Deps: %i, Versions: %i\n", (*manifest)[i]->deps_sz, (*manifest)[i]->versions_sz);

            if (o.find("deps") != o.end()) {
                if (pico_to_arr(o, "deps", &(*manifest)[i]->deps, &(*manifest)[i]->deps_sz) < 0) {
                    // free(latest);
                    // free(name);
                    // free(source);
                    // free(repo);
                    // free(uri);
                    // manifest_array_destruct(manifest, i + 1);
                    free((*manifest)[i]->deps);
                }
            } else {
                deps_sz = 0;
            }

            if (o.find("versions") != o.end()) {
                if (pico_to_arr(o, "versions", &(*manifest)[i]->versions, &(*manifest)[i]->versions_sz) < 0) {
                    // free(latest);
                    // free(name);
                    // free(source);
                    // free(repo);
                    // free(uri);
                    // manifest_array_destruct(manifest, i + 1);
                    free((*manifest)[i]->versions);
                    // return -1;
                }
            } else {
                versions_sz = 0;
            }

            if (latest == nullptr) { // need to check breakpoint here
                free(latest);
                free(name);
                free(uri);
                free(source);
                free(repo);
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            (*manifest)[i]->latest = (SemVer*)malloc(sizeof(SemVer));
            memcpy((*manifest)[i]->latest, latest, sizeof(SemVer));

            strncpy((*manifest)[i]->uri, uri, sizeof((*manifest)[i]->uri) - 1);
            (*manifest)[i]->uri[sizeof((*manifest)[i]->uri) - 1] = '\0';

            strncpy((*manifest)[i]->uri_interpolated, uri, sizeof((*manifest)[i]->uri_interpolated) - 1);
            (*manifest)[i]->uri_interpolated[sizeof((*manifest)[i]->uri_interpolated) - 1] = '\0';

            if (uri_env((*manifest)[i]->uri_interpolated, (*manifest)[i]->name, (*manifest)[i]->latest->string) < 0) {
                free(latest);
                free(name);
                free(uri);
                free(source);
                free(repo);
                manifest_array_destruct(manifest, i + 1);
                return -1;
            }

            memcpy((*manifest)[i]->source, source, strlen(source) + 1);

            if (strlen(repo) > 0) {
                strncpy((*manifest)[i]->repo, repo, sizeof((*manifest)[i]->repo) - 1);
                (*manifest)[i]->repo[sizeof((*manifest)[i]->repo) - 1] = '\0';
            }

            // manifest_dump((*manifest)[i]);
            
            free(name);
            free(uri);
            free(source);
            free(repo);
            free(latest);
        }

        manifest_size = arr.size();

        // manifest_array_destruct(*manifest, arr.size());

    } else {
        RED("No manifest detected!\n");
        return -1;
    }

    return manifest_size;
}

// code for path parsing and extension checks may be helpful for .tar.gz, .zip, .git, etc.
            // char* ext = (char*)malloc(32);
            // if (last_of(ext, (*manifest)[i]->uri, '/') < 0) {
            //     BRED("Last_of failed on string %s\n", (*manifest)[i]->uri);
            //     return -1;
            // }

            // YEL("Ext is: %s\n", ext);
            
            // char* repo = (char*)malloc(32);

            // if (strcmp(source, "github-clone") == 0) {

            //     char* p = strchr(ext, '.');
            //     if (p != NULL) {
            //         strncpy(repo, ext, p - ext);
            //         repo[p - ext] = 0;
            //     }

            //     YEL("Repo is: %s\n", repo);

            // }

            // if (strlen(repo) == 0) {
            //     BRED("Empty repo name!\n");
            // }