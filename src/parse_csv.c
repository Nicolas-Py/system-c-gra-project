//
// Created by Ziang Liu on 07.07.24.
//


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_csv.h"
#include <ctype.h>

#define MAX_REQUESTS 100000
#define MAX_LINE_LENGTH 100000


// Function to check if a string is a valid decimal or hexadecimal number
int isValidNumber(const char *str) {
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        // check for hexadecimal validity
        for (int i = 2; str[i] != '\0'; i++) {
            if (!isxdigit((unsigned char)str[i])) {
                return 0;
            }
        }
    } else {
        // check for decimal validity
        for (int i = 0; str[i] != '\0'; i++) {
            if (!isdigit((unsigned char)str[i])) {
                return 0;
            }
        }
    }
    return 1;
}

// Function to convert string to uint32_t
uint32_t stringToUint32(const char *str) {
    uint32_t toReturn;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        toReturn = (uint32_t)strtoul(str, NULL, 16);
    } else {
        toReturn = (uint32_t)strtoul(str, NULL, 10);
    }
    return toReturn;
}

struct Request* parse_csv(const char* path, size_t* count) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file.\n");
        return NULL;
    }

    struct Request* requests = malloc(MAX_REQUESTS * sizeof(struct Request));
    if (requests == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }
    //using char line[] creates a so called buffer to read line by line
    char line[MAX_LINE_LENGTH];
    int lineNumber = 0;
    *count = 0;

    while (fgets(line, sizeof(line), file) && *count < MAX_REQUESTS) {
        lineNumber++;
        char *token;
        int column = 0;
        char firstColumn[2] = {0};
        char secondColumn[MAX_LINE_LENGTH] = {0};
        char thirdColumn[MAX_LINE_LENGTH] = {0};

        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }

        token = strtok(line, ",");
        while (token != NULL) {
            // Remove leading/trailing whitespace
            char *trimmed = token;
            while (isspace(*trimmed)) trimmed++;
            char *end = trimmed + strlen(trimmed) - 1;
            while (end > trimmed && isspace(*end)) end--;
            *(end + 1) = '\0';

            //Each line may not have more than three columns
            if (column >= 3) {
                printf("Error on line %d: Too many columns.\n", lineNumber);
                free(requests);
                fclose(file);
                return NULL;
            }

            switch (column) {
                case 0:
                    if (strcmp(trimmed, "R") != 0 && strcmp(trimmed, "W") != 0) {
                        printf("Error on line %d: First column must be 'R' or 'W'.\n", lineNumber);
                        free(requests);
                        fclose(file);
                        return NULL;
                    }
                    strcpy(firstColumn, trimmed);
                    break;
                case 1:
                    if (!isValidNumber(trimmed)) {
                        printf("Error on line %d: Second column must be a valid decimal or hexadecimal number.\n", lineNumber);
                        free(requests);
                        fclose(file);
                        return NULL;
                    }
                    strcpy(secondColumn, trimmed);
                    break;
                case 2:
                    if (strcmp(firstColumn, "R") == 0 && strlen(trimmed) != 0) {
                        printf("Error on line %d: Third column must be empty when first column is 'R'.\n", lineNumber);
                        free(requests);
                        fclose(file);
                        return NULL;
                    }
                    if (strcmp(firstColumn, "W") == 0 && !isValidNumber(trimmed)) {
                        printf("Error on line %d: Third column must be a valid decimal or hexadecimal number when first column is 'W'.\n", lineNumber);
                        free(requests);
                        fclose(file);
                        return NULL;
                    }
                    strcpy(thirdColumn, trimmed);
                    break;
            }
            token = strtok(NULL, ",");
            column++;
        }

        if (column < 2 || (strcmp(firstColumn, "W") == 0 && column != 3)) {
            printf("Error on line %d: Incorrect number of columns.\n", lineNumber);
            free(requests);
            fclose(file);
            return NULL;
        }

        // If all checks pass, add the request to the array
        requests[*count].we = (strcmp(firstColumn, "W") == 0) ? 1 : 0;
        requests[*count].addr = stringToUint32(secondColumn);
        requests[*count].data = (requests[*count].we) ? stringToUint32(thirdColumn) : 0;

        (*count)++;
    }
    fclose(file);
    return requests;
}