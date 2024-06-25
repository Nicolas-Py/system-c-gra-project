//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//
#include "modules.hpp"
#include <systemc>


struct Request {
     uint32_t addr ;
     uint32_t data ;
     int we ;
};
struct Result {
	size_t cycles ;
 	size_t misses ;
 	size_t hits ;
 	size_t primitiveGateCount;
};

struct Result run_simulation (
	int cycles,
	int directMapped,
	unsigned cacheLines,
	unsigned cacheLineSize,
	unsigned cacheLatency,
	unsigned memoryLatency,
	size_t numRequests,
	struct Request requests[numRequests],
	const char * tracefile
)
{
    return 1;
}

int sc_main(int argc, char* argv[]) {
    std::cout << "ERROR" << std::endl;
    return 1;
}