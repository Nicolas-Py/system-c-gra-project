//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//

#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include "systemc.h"
using namespace sc_core;

struct Request {
     uint32_t addr;
     uint32_t data;
     int we;
};

//own Memory request definition to be able to use sc_signal<Memory Request>
struct MEMORY_REQUEST {
	uint32_t addr;
	uint32_t data;
	int we;
	int requestNum;

	MEMORY_REQUEST(): addr(0), data(0), we(0), requestNum(0){}
	MEMORY_REQUEST(uint32_t addr, uint32_t data, int we, int requestNum): addr(addr), data(data), we(we), requestNum(requestNum) {}

	//overwritten the assignment operator
	MEMORY_REQUEST& operator=(const MEMORY_REQUEST& request) {
		addr = request.addr;
		data = request.data;
		we = request.we;
		requestNum = request.requestNum;
		return *this;
	}

	//overitten the equality check opertor
	bool operator==(const MEMORY_REQUEST& request) const {
		return addr == request.addr && data == request.data && we == request.we && requestNum == request.requestNum;
	}
};

// defined output for printing for example for std::cout on a sc_signal<MEMORY_REQUEST>
inline std::ostream& operator<<(std::ostream& output, const MEMORY_REQUEST& request) {
	output << "Request: " << request.addr << "\nData: " << request.data << "\nwe: " << request.we << "\nRequestNum: " << request.requestNum;
	return output;
}

//defined how sc_trace should trace an sc_signal<Block>
inline void sc_trace(sc_trace_file* file, const MEMORY_REQUEST& request, const std::string& name) {
	sc_trace(file, request.addr, name + ".addr");
	sc_trace(file, request.data, name + ".data");
	sc_trace(file, request.we, name + ".we");
	sc_trace(file, request.requestNum, name + ".requestNum");
}

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
