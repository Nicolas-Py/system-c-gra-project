//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//
#include "modules.hpp"
#include <systemc>


Result run_simulation (
	int cycles,
	int directMapped,
	unsigned cacheLines,
	unsigned cacheLineSize,
	unsigned cacheLatency,
	unsigned memoryLatency,
	size_t numRequests,
	Request requests[numRequests],
	const char * tracefile
)
{
	Result testResult = {1, 1, 1, 1};
    return testResult;
}

int sc_main(int argc, char* argv[]) {
    std::cout << "ERROR" << std::endl;
    return 1;
}