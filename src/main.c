#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "parse_csv.c"
#include "print_usage.c"
#include "limits.h"

struct Result {
	size_t cycles;
	size_t misses;
	size_t hits;
	size_t primitiveGateCount;
};


extern struct Result run_simulation (
	int cycles,
	int directMapped,
	unsigned cacheLines,
	unsigned cacheLineSize,
	unsigned cacheLatency,
	unsigned memoryLatency,
	size_t numRequests,
	struct Request requests[numRequests],
	const char* tracefile
);





int main(int argc, char* argv[]) {
    // Default values
    int cycles = 10000;  // Default number of cycles
    int directMapped = 1;  // Default to direct-mapped cache
    unsigned cacheLines = 8;  // Default number of cache lines
    unsigned cacheLineSize = 32;  // Default cache line size in bytes
    unsigned cacheLatency = 5;  // Default cache latency in cycles
    unsigned memoryLatency = 100;  // Default memory latency in cycles
    char* tracefile = NULL;
    char* inputfile = NULL;

    // Long options for getopt_long
    static struct option long_options[] = {
        {"cycles", required_argument, 0, 'c'},
        {"directmapped", no_argument, 0, 'd'},
        {"fullassociative", no_argument, 0, 'f'},
        {"cacheline-size", required_argument, 0, 's'},
        {"cachelines", required_argument, 0, 'l'},
        {"cache-latency", required_argument, 0, 'a'},
        {"memory-latency", required_argument, 0, 'm'},
        {"tf", required_argument, 0, 't'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;
    int d_flag = 0;
    int f_flag = 0;

    while ((c = getopt_long(argc, argv, "c:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'c': {
                errno = 0;
                char* endptr; // checks for non decimals
                long tempCycles = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || tempCycles <= 0 || *endptr != '\0'|| tempCycles > INT_MAX) {
                    fprintf(stderr, "Error: Number of cycles must be a positive integer.\n");
                    exit(1);
                }
                cycles = (int) tempCycles;
                break;
            }

            case 'd':
                if (f_flag) {
                    fprintf(stderr, "Error: You can only select either --directmapped or --fullassociative, not both.\n");
                    exit(1);
                }
                directMapped = 1;
                d_flag = 1;
                break;

            case 'f':
                if (d_flag) {
                    fprintf(stderr, "Error: You can only select either --directmapped or --fullassociative, not both.\n");
                    exit(1);
                }
                directMapped = 0;
                f_flag = 1;
                break;

            case 's': {
                errno = 0;
                char* endptr; // checks for non decimals
                long tempSize = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || tempSize <= 0 || *endptr != '\0' || tempSize > UINT_MAX) {
                    fprintf(stderr, "Error: Cache line size must be a positive integer.\n");
                    exit(1);
                }
                cacheLineSize = (unsigned) tempSize;
                break;
            }

            case 'l': {
                errno = 0;
                char* endptr; // checks for non decimals
                long tempLines = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || tempLines <= 0 || *endptr != '\0' || tempLines > UINT_MAX) {
                    fprintf(stderr, "Error: Number of cache lines must be a positive integer.\n");
                    exit(1);
                }
                cacheLines = (unsigned) tempLines;
                break;
            }

            case 'a': {
                errno = 0;
                char* endptr; // checks for non decimals
                long tempCacheLatency = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || tempCacheLatency <= 0 || *endptr != '\0' || tempCacheLatency > UINT_MAX) {
                    fprintf(stderr, "Error: Cache latency must be a positive integer.\n");
                    exit(1);
                }
                cacheLatency = (unsigned) tempCacheLatency;
                break;
            }

            case 'm': {
                errno = 0;
                char* endptr; // checks for non decimals
                long tempMemoryLatency = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || tempMemoryLatency <= 0 || *endptr != '\0' || tempMemoryLatency > UINT_MAX) {
                    fprintf(stderr, "Error: Memory latency must be a positive integer.\n");
                    exit(1);
                }
                memoryLatency = (unsigned) tempMemoryLatency;
                break;
            }

            case 't':
                tracefile = optarg;
                if (access(tracefile, F_OK) == -1) {
                    fprintf(stderr, "Error: Tracefile '%s' does not exist.\n", tracefile);
                    exit(1);
                }
                break;

            case 'h':
                print_usage(argv[0]);
                exit(0);

            case '?':
                // Unrecognized option, getopt_long already prints an error message.
                exit(1);

            default:
                abort();
        }
    }


    // Check for input file (positional argument)
    if (optind < argc) {
        // inputfile represents the path
        inputfile = argv[optind];
    } else {
        fprintf(stderr, "Error: Input file is required.\n");
        print_usage(argv[0]);
        exit(1);
    }

    size_t num_requests;
    struct Request* requests = parse_csv(inputfile, &num_requests);
    if (requests == NULL) {
        fprintf(stderr, "invalid CSV format");
        return -1;
    }


    // Run the simulation
    printf("directMapped %d\n", directMapped);
    struct Result res = run_simulation(cycles, directMapped, cacheLines, cacheLineSize,
                                       cacheLatency, memoryLatency, num_requests, requests, tracefile);

    // Print results
    printf("Simulation Results:\n");
    printf("Cycles: %zu\n", res.cycles);
    printf("Cache hits: %zu\n", res.hits);
    printf("Cache misses: %zu\n", res.misses);
    printf("Primitive gate count: %zu\n", res.primitiveGateCount);

    free(requests);

    return 0;
}