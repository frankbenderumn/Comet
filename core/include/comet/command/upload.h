#ifndef COMET_COMMAND_UPLOAD_H_
#define COMET_COMMAND_UPLOAD_H_

int upload(int argc, char* argv[]) {

    if (argc != 4) {
        BRED("Invalid number of arguments for command upload!\n");
        return -1;
    }

    char* script = argv[2];
    char* signature = argv[3];
    const char* package_dir = "./.internal/package/";
    char* package_ptr = NULL;

    printf("Script path: %s\n", script);
    printf("Signature: %s\n", signature);

    package_ptr = strrchr(script, '/');

    if (package_ptr == NULL) {
        BRED("Package name contains no '/'\n");
        return -1;
    }

    char package_name[32];
    strncpy(package_name, package_ptr + 1, 32);

    printf("Package name: %s\n", package_name);

    if (!exists(script)) {
        BRED("Invalid script provided for upload!\n");
        return -1;
    }

    long sz;
    bytes_t script_bin = read_bin(script, &sz);
    bytes_t write_bin = (bytes_t)malloc(sizeof(BYTE) * (sz + 64));
    BYTE sig[] = {"howdsdfsdfsdfsdfsdfsdf"};
    if (pad(sig, 64) < 0) {
        BRED("Failed to pad signature!\n");
        free(write_bin);
        free(script_bin);
        return -1;
    }

    memcpy(write_bin, sig, 64);
    memcpy(write_bin + 64, script_bin, sz);

    char* package_path = (char*)malloc(sizeof(char) * (strlen(package_dir) + strlen(package_name) + 1));

    strncpy(package_path, package_dir, strlen(package_dir));
    strncpy(package_path + strlen(package_dir), package_name, strlen(package_name));
    package_path[strlen(package_dir) + strlen(package_name)] = 0;

    printf("Package_path: %s\n", package_path);

    FILE* fp = fopen(package_path, "wb");

    if (!fp) {
        BRED("Failed to write to file: %s\n", package_path);
        free(write_bin);
        free(script_bin);
        free(package_path);
        fclose(fp);
        return -1;
    }

    fwrite(write_bin, 1, sz + 64, fp);

    fclose(fp);
    free(script_bin);
    free(write_bin);
    return 1;
}

#endif