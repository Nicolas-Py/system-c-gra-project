//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//

#ifndef MODULES_HPP
#define MODULES_HPP

#include <cstdint>
#include <systemc>
#include "systemc.h"
using namespace sc_core;

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

extern "C" struct Result run_simulation (
	int cycles,
	int directMapped,
	unsigned cacheLines,
	unsigned cacheLineSize,
	unsigned cacheLatency,
	unsigned memoryLatency,
	size_t numRequests,
	Request requests[numRequests],
	const char * tracefile
);

#endif
