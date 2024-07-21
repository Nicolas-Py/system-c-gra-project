//
// Created by Ziang Liu on 07.07.24.
//


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_csv.h"

#include <errno.h>

#include <ctype.h>

#define MAX_REQUESTS 100000
#define MAX_LINE_LENGTH 100000


size_t line_count(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    size_t count = 0;
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), file)) {
        count++;
    }

    fclose(file);
    return count;

}

int is_valid(const char* str) {
    char* endptr;
    errno = 0;
    strtoul(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0') {
        return 0;
    }
    return 1;
}

int is_valid_hex_or_decimal(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    int is_hex = 0;
    int i = 0;

    // Check for hexadecimal prefix
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        is_hex = 1;
        i = 2;
    }
    fprintf(stderr, "ishex %d\n", is_hex);
    // Iterate through the remaining characters
    for (; str[i] != '\0' || isxdigit(str[i]) != 10; i++) {
        if (is_hex) {
            fprintf(stderr, "char %d\n", str[i]);
            if (!isxdigit(str[i])) {
                return 0;
            }
        } else {
            if (!isdigit(str[i])) {
                return 0;
            }
        }
    }

    return 1;
}




int is_empty_or_whitespace(const char* str) {
    while (*str != '\0') {
        if (!isspace(*str)) {
            return 0; // Not empty or whitespace
        }
        str++;
    }
    return 1; // Empty or whitespace
}



// Function to check if a string is a valid decimal or hexadecimal number
int isValidNumber(const char *str) {
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        // Hexadecimal
        for (int i = 2; str[i] != '\0'; i++) {
            if (!isxdigit((unsigned char)str[i])) {
                return 0;
            }
        }
    } else {
        // Decimal
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
            while (isspace((unsigned char)*trimmed)) trimmed++;
            char *end = trimmed + strlen(trimmed) - 1;
            while (end > trimmed && isspace((unsigned char)*end)) end--;
            *(end + 1) = '\0';

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









/*
struct Request * parse_csv(const char* path, size_t* num_requests) {

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    *num_requests = line_count(path);
    struct Request* requests = (struct Request *)malloc(*num_requests * sizeof(struct Request));

    if (requests == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET);
    char buffer[1024];
    size_t index = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        char* operation = strtok(buffer, ",");
        char* addr_str = strtok(NULL, ",");
        char* data_str = strtok(NULL, ",");
        printf("operation: %s, address: %s, data: %s", operation, addr_str, data_str);

        if (operation == NULL || addr_str == NULL) {
            fprintf(stderr, "Invalid CSV format at line %zu\n", index + 1);
            return NULL;
        }

        if (strcmp(operation, "W") == 0) {
            requests[index].we = 1;
            if (data_str == NULL || !is_valid(data_str)) {
                fprintf(stderr, "Invalid data or missing data for write operation at line %zu\n", index + 1);
                return NULL;
            }
            requests[index].data = (uint32_t)strtoul(data_str, NULL, 0);
        } else if (strcmp(operation, "R") == 0) {
            requests[index].we = 0;
            if (data_str != NULL && strlen(data_str) > 0) {
                fprintf(stderr, "Data provided for read operation at line %zu, should be empty!\n", index + 1);
                return NULL;
            }
            requests[index].data = 0;
        } else {
            fprintf(stderr, "Unknownn operation '%s' at line %zu\n", operation, index + 1);
            return NULL;
        }

        if (!is_valid(addr_str)) {
            fprintf(stderr, "Invalid address '%s' at line %zu\n", addr_str, index + 1);
            return NULL;
        }
        requests[index].addr = (uint32_t)strtoul(addr_str, NULL, 0);
        index++;
    }

    fclose(file);
    return requests;

}
*/