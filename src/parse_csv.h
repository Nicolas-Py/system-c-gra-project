//
// Created by Ziang Liu on 07.07.24.
//

#ifndef FILE_H
#define FILE_H
struct Request {
    uint32_t addr ;
    uint32_t data ;
    int we ;
};
size_t line_count(const char* path);
struct Request* parse_csv(const char* path, size_t* num_requests);

#endif