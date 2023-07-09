#ifndef COMET_COMMAND_UPDATE_TOKEN_H_ 
#define COMET_COMMAND_UPDATE_TOKEN_H_ 

#include <unistd.h>

void update_token(const char* key, const char* val) {

    char* TOKEN_PATH = getenv("TOKEN_PATH");

    if (TOKEN_PATH == NULL) {
        printf("\033[1;31mCritical:\033[0m Token config file not assigned. Program corrupted!\n");
        return;
    }

    if (access(TOKEN_PATH, F_OK) != -1) {

        FILE* fp_read;
        if ((fp_read = fopen(TOKEN_PATH, "r")) == NULL) {
            BRED("Failed to open file '%s' for reading\n", TOKEN_PATH);
        }

        // Create a temporary file to write the updated contents
        const char* temp_file_path = "./config/token_temp";
        FILE* temp_file = fopen(temp_file_path, "w");
        if (temp_file == NULL) {
            printf("Failed to create temporary file.\n");
            fclose(fp_read);
            return;
        }

        char line[100];
        while (fgets(line, sizeof(line), fp_read) != NULL) {
            char* parsed_key = strtok(line, "=");
            char* parsed_value = strtok(NULL, "=");

            if (parsed_key != NULL && parsed_value != NULL) {
                // printf("Key: %s\n", parsed_key);
                // printf("Value: %s\n", parsed_value);
                if (strcmp(parsed_key, key) == 0) {
                    // MAG("Let's update key!\n");
                    fprintf(temp_file, "%s=%s\n", parsed_key, val);
                } else {
                    fprintf(temp_file, "%s=%s", parsed_key, parsed_value);
                }
            }

        }

        fclose(fp_read);
        fclose(temp_file);

        // Replace the original file with the updated file
        if (remove(TOKEN_PATH) != 0) {
            printf("Failed to remove original file.\n");
            return;
        }
        if (rename(temp_file_path, TOKEN_PATH) != 0) {
            printf("Failed to rename temporary file.\n");
            return;
        }

    } else {
        BRED("Token file has not been initiated!\n");
        return;
    }

}

#endif