#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>


struct Request {
	uint32_t addr ;
	uint32_t data ;
	int we ;
};
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



// Function to print usage information
void print_usage(const char* program_name) {
    fprintf(stderr, "Usage: %s [OPTIONS] <input_file>\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c, --cycles <num>           Number of cycles to simulate\n");
    fprintf(stderr, "  --directmapped               Simulate a direct-mapped cache\n");
    fprintf(stderr, "  --fullassociative            Simulate a fully associative cache\n");
    fprintf(stderr, "  --cacheline-size <num>       Size of a cache line in bytes\n");
    fprintf(stderr, "  --cachelines <num>           Number of cache lines\n");
    fprintf(stderr, "  --cache-latency <num>        Cache latency in cycles\n");
    fprintf(stderr, "  --memory-latency <num>       Main memory latency in cycles\n");
    fprintf(stderr, "  --tf=<filename>              Output tracefile with all signals\n");
    fprintf(stderr, "  -h, --help                   Display this help message\n");
}

int main(int argc, char* argv[]) {
    // Default values
    int cycles = 1000;  // Default number of cycles
    int directMapped = 1;  // Default to direct-mapped cache
    unsigned cacheLines = 64;  // Default number of cache lines
    unsigned cacheLineSize = 64;  // Default cache line size in bytes
    unsigned cacheLatency = 1;  // Default cache latency in cycles
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
            case 'c':
                cycles = atoi(optarg);
                break;
            case 'd':
                if (f_flag) {
                    fprintf(stderr, "You can only select either --directmapped or --fullassociative not both\n");
                    exit(1);
                }
                directMapped = 1;
                d_flag = 1;
                break;
            case 'f':
                if (d_flag) {
                    fprintf(stderr, "You can only select either --directmapped or --fullassociative not both\n");
                    exit(1);
                }
                directMapped = 0;
                f_flag = 1;
                break;
            case 's':
                cacheLineSize = atoi(optarg);
                break;
            case 'l':
                cacheLines = atoi(optarg);
                break;
            case 'a':
                cacheLatency = atoi(optarg);
                break;
            case 'm':
                memoryLatency = atoi(optarg);
                break;
            case 't':
                tracefile = optarg;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case '?':
                exit(1);
            default:
                abort();
        }
    }

    // Check for input file (positional argument)
    if (optind < argc) {
        inputfile = argv[optind];
    } else {
        fprintf(stderr, "Error: Input file is required.\n");
        print_usage(argv[0]);
        exit(1);
    }

    // TODO: Read requests from inputfile
    // For now, we'll use an empty array
    struct Request requests[] = {};
    size_t numRequests = 0;

    // Run the simulation
    struct Result res = run_simulation(cycles, directMapped, cacheLines, cacheLineSize,
                                       cacheLatency, memoryLatency, numRequests, requests, tracefile);

    // Print results
    printf("Simulation Results:\n");
    printf("Cycles: %zu\n", res.cycles);
    printf("Cache hits: %zu\n", res.hits);
    printf("Cache misses: %zu\n", res.misses);
    printf("Primitive gate count: %zu\n", res.primitiveGateCount);

    return 0;
}