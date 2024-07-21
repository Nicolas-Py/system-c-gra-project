//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//
#include "modules.hpp"
#include "direct-mapped-cache.hpp"
#include "full_associative_cache.hpp"
#include "main-memory.hpp"
# include <map>


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
	if (directMapped) {
		printf("DIRECT_MAPPED_CACHE\n");
		DIRECT_MAPPED_CACHE direct("direct", cacheLines, cacheLineSize, sizeof(uint32_t));
		sc_signal<MEMORY_REQUEST> request;
		sc_signal<int> hit;

		size_t hits = 0;
		size_t misses = 0;
		size_t cyclesCount = 0;

		direct.request(request);
		direct.out(hit);

		for (int i = 0; i < numRequests; i++) {
			printf("Request number: %u, address: %u, data: %u, write/enable: %d\n", i + 1, requests[i].addr, requests[i].data, requests[i].we);
			request.write(MEMORY_REQUEST{requests[i].addr, requests[i].data, requests[i].we, i});
			sc_start(1, SC_SEC);
			if (hit.read()) {
				hits++;
				cyclesCount += cacheLatency;
			} else {
				misses++;
				cyclesCount += cacheLatency;
				cyclesCount += memoryLatency;
			}
			if (cyclesCount>cycles) {
				cyclesCount = SIZE_MAX;
				break;
			}
		}
		Result testResult = {cyclesCount, misses, hits, 1};
		return testResult;
	} else {
		printf("ASSOCIATIVE_CACHE\n");
		ASSOCIATIVE_CACHE associative_cache("associative", cacheLines, cacheLineSize, sizeof(uint32_t));
		sc_signal<MEMORY_REQUEST> request;
		sc_signal<int> hit;

		size_t misses = 0;
		size_t hits = 0;
		size_t cyclesCount = 0;

		associative_cache.request(request);
		associative_cache.out(hit);

		for (int i = 0; i < numRequests; i++) {
			printf("Request number: %u, address: %u, data: %u, write/enable: %d\n", i + 1, requests[i].addr, requests[i].data, requests[i].we);
			request.write(MEMORY_REQUEST{requests[i].addr, requests[i].data, requests[i].we, i});
			sc_start(1, SC_SEC);
			if (hit.read()) {
				hits++;
				cyclesCount += cacheLatency;
			} else {
				misses++;
				cyclesCount += cacheLatency;
				cyclesCount += memoryLatency;
			}
			if (cyclesCount>cycles) {
				cyclesCount = SIZE_MAX;
				break;
			}

		}
		Result testResult = {cyclesCount, misses, hits, 1};
		return testResult;
	}
}


int sc_main(int argc, char* argv[]) {
	std::cout << "ERROR" << std::endl;
	return 0;
}