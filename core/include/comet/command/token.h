#ifndef COMET_COMMAND_TOKEN_H_ 
#define COMET_COMMAND_TOKEN_H_ 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void token(const char* key, const char* val) {

    char* TOKEN_PATH = getenv("TOKEN_PATH");

    if (TOKEN_PATH == NULL) {
        printf("\033[1;31mCritical:\033[0m Token config file not assigned. Program corrupted!\n");
        return;
    }

    char* toWrite = (char*)malloc(strlen(key) + strlen(val) + 2);

    #define MAX_ITEMS 100
    #define MAX_KEY_LENGTH 50

    typedef struct {
        char key[MAX_KEY_LENGTH];
        int count;
    } KeyCount;

    KeyCount keyCounts[MAX_ITEMS];
    int itemCount = 0;

    if (access(TOKEN_PATH, F_OK) != -1) {

        FILE* fp_read;
        if ((fp_read = fopen(TOKEN_PATH, "r")) == NULL) {
            BRED("Failed to open file '%s' for reading\n", TOKEN_PATH);
        }

        char line[100];
        while (fgets(line, sizeof(line), fp_read) != NULL) {
            line[strcspn(line, "\n")] = '\0'; // Remove trailing newline character
            // printf("Line: %s\n", line);
            int eq_count = 0;
            for (int i = 0; line[i] != '\0'; i++) {
                if (line[i] == '=') eq_count++;
            }
            if (eq_count > 1) {
                BRED("Too many equal characters token in token!\n");
                fclose(fp_read);
                exit(1);
            }
            char* p = strstr(line, "=");
            if (p == NULL) {
                BRED("Corrupted data detected in token file. No = found in line!\n");
                fclose(fp_read);
                exit(1);
            }
            char* key = strtok(line, "=");
            char* value = strtok(NULL, "=");

            // if (key != NULL && value != NULL) {
            //     printf("Key: %s\n", key);
            //     printf("Value: %s\n", value);
            // }

            // Find if the key already exists in the keyCounts array
            int keyIndex = -1;
            for (int i = 0; i < itemCount; i++) {
                if (strcmp(keyCounts[i].key, key) == 0) {
                    BRED("Key '%s' already exists! Please use update_token instead!\n", key);
                    exit(1);
                    keyIndex = i;
                    break;
                }
            }

            // Update the count for an existing key or add a new key
            if (keyIndex != -1) {
                keyCounts[keyIndex].count++;
            } else if (itemCount < MAX_ITEMS) {
                strcpy(keyCounts[itemCount].key, key);
                keyCounts[itemCount].count = 1;
                itemCount++;
            } else {
                printf("Reached the maximum number of items.\n");
                break;
            }
        }
        fclose(fp_read);

    }

    FILE* fp;
    if ((fp = fopen(TOKEN_PATH, "a")) == NULL) {
        BRED("Failed to open file '%s'\n", TOKEN_PATH);
        return;
    }

    int keyIndex = -1;
    for (int i = 0; i < itemCount; i++) {
        if (strcmp(keyCounts[i].key, key) == 0) {
            BRED("Key '%s' already exists! Please use update_token instead!\n", key);
            fclose(fp);
            exit(1);
            keyIndex = i;
            break;
        }
    }

    memcpy(toWrite, key, strlen(key));
    memcpy(toWrite + strlen(key), "=", 1);
    memcpy(toWrite + strlen(key) + 1, val, strlen(val));
    toWrite[strlen(key) + strlen(val) + 1] = 0;

    fprintf(fp, "%s\n", toWrite);

    fclose(fp);


}

#endif