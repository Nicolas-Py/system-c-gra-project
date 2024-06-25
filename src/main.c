#include <stdio.h>
#include <stdlib.h>


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




int main(int argc, char* argv[]) {
	struct Request requests[] = {};
	struct Result res = run_simulation(2, 0, 4, 4, 4, 4, 0, requests, "");
	printf("%lu\n", res.cycles);
    return 1;
}