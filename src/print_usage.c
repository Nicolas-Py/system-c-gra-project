#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
// Function to print usage information
void print_usage(const char* program_name) {

    #define CYAN    "\x1b[36m"
    #define YELLOW  "\x1b[33m"
    #define RESET   "\x1b[0m"
    #define MAG "\x1B[35m"

    fprintf(stderr, CYAN"\n\n\n\n ░▒▓██████▓▒░ ░▒▓██████▓▒░ ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░░▒▓███████▓▒░▒▓█▓▒░▒▓██████████████▓▒░  \n");
    fprintf(stderr, "░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ \n");
    fprintf(stderr, "░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ \n");
    fprintf(stderr, "░▒▓█▓▒░      ░▒▓████████▓▒░▒▓█▓▒░      ░▒▓████████▓▒░▒▓██████▓▒░  ░▒▓██████▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ \n");
    fprintf(stderr, "░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░             ░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ \n");
    fprintf(stderr, "░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░             ░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ \n");
    fprintf(stderr, " ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓███████▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░ \n\n\n\n"RESET);



    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  program call                 %s "MAG"[OPTIONS]"RESET" "YELLOW"<input_file>"RESET"\n", program_name);
    fprintf(stderr, "  "MAG"[OPTIONS]"RESET"                    Possible arguemts to \n");
    fprintf(stderr, "  "YELLOW"<input_file>"RESET"                 the path to the inputfile\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c, --cycles <num>           Number of cycles to simulate\n");
    fprintf(stderr, "  --directmapped               Simulate a direct-mapped cache\n");
    fprintf(stderr, "  --fullassociative            Simulate a fully associative cache\n");
    fprintf(stderr, "  --cacheline-size <num>       Size of a cache line in bytes\n");
    fprintf(stderr, "  --cachelines <num>           Number of cache lines\n");
    fprintf(stderr, "  --cache-latency <num>        Cache latency in cycles\n");
    fprintf(stderr, "  --memory-latency <num>       Main memory latency in cycles\n");
    fprintf(stderr, "  --tf=<filename>              Output tracefile with all signals\n");
    fprintf(stderr, "  -h, --help                   Display this help message\n\n");
    fprintf(stderr, "Default values:\n");
    fprintf(stderr, "  Cycles:                      %d\n", 1000);
    fprintf(stderr, "  Directmapped:                %s\n", "1 - So a directmapped cache gets simulated");
    fprintf(stderr, "  Cacheline Size:              %d\n", 64);
    fprintf(stderr, "  Number of Cachelines:        %d\n", 64);
    fprintf(stderr, "  Cache-latency:               %d\n", 1);
    fprintf(stderr, "  Memory-latency:              %d\n", 64);
    fprintf(stderr, "  Tracefile:                   %s\n", "No default value, if this argument is not");
    fprintf(stderr, "                               %s\n", "provided no tracefile will be created");
}