//
// Created by Ziang Liu on 07.07.24.
//


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

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

struct Request* parse_csv(const char* path, size_t* num_requests) {

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }


    *num_requests = line_count(path);
    struct Request* requests = (struct Request *)malloc(*num_requests);

    if (requests == NULL) {
        perror("Failed to allocate memory");
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
            continue; // Skip invalid lines
        }

        if (strcmp(operation, "W") == 0) {
            requests[index].we = 1;
        } else if (strcmp(operation, "R") == 0) {
            requests[index].we = 0;
        } else {
            fprintf(stderr, "Unknownn operation '%s' at line %zu\n", operation, index + 1);
        }


        requests[index].addr = (uint32_t)strtoul(addr_str, NULL, 0);
        requests[index].data = (data_str) ? (uint32_t)strtoul(data_str, NULL, 0) : 0;
        index++;
    }

    fclose(file);
    return requests;

}

int main(int argc, char* argv[]) {
    size_t num_requests;
    struct Request* requests = parse_csv("src/input.csv", &num_requests);
    for (int i = 0; i < num_requests; i++) {
        printf("Request number: %u, address: %u, data: %u, write/enable: %d\n", i + 1, requests[i].addr, requests[i].data, requests[i].we);
    }
    free(requests);
    return 0;
}