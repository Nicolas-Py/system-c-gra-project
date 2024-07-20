//
// Created by Ziang Liu on 07.07.24.
//


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_csv.h"

#include <errno.h>


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
    errno = 0;
    strtoul(str, NULL, 0);
    if (errno != 0) {
        return 0; // Invalid number
    }
    return 1; // Valid number
}



struct Request* parse_csv(const char* path, size_t* num_requests) {

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