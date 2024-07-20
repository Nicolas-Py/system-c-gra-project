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

struct Memory {
	typedef uint32_t Address;
	typedef uint32_t Tag;
	typedef uint32_t Index;
	typedef uint32_t Offset;

	uint32_t cacheLineSize;
	uint32_t returnAmount;
	uint32_t offsetBitAmount;
	uint32_t entrySize;

	std::unordered_map<uint32_t, std::vector<uint32_t>> memory;

	Memory(uint32_t cacheLineSize, uint32_t returnAmount, uint32_t entrySize):
		cacheLineSize(cacheLineSize),
		returnAmount(returnAmount),
		offsetBitAmount(static_cast<int>(std::ceil(std::log2(cacheLineSize)))),
		entrySize(entrySize)
	{}

	std::vector<uint32_t> insert(Request request) {
		Address tag_index = request.addr >> offsetBitAmount;
		Offset offset = request.addr << (32-offsetBitAmount);
		offset >>= (32-offsetBitAmount);

		//Testing:
		std::bitset<32> TAG_INDEX(tag_index);
		std::bitset<32> Offset(offset);
		std:: cout << "\nMemory stuff: " << std::endl;
		std::cout << "	Tag_Index: " << TAG_INDEX << " | Decimal: " << tag_index << std::endl;
		std::cout << "	Offset: " << Offset << " | Decimal: " << offset << std::endl;

		//test end


		if (memory.find(tag_index) == memory.end()) {
			memory[tag_index] = std::vector<uint32_t>(cacheLineSize, 0);
		}
		if (request.we) {
			memory[tag_index][offset] = request.data;
		}

		printMemory();
		return extractBlock(offset, tag_index);
	}

	std::vector<uint32_t> extractBlock(Offset offset, Address tag_index) {
		std::vector<uint32_t>& block = memory[tag_index];
		return {block.begin() + offset - (offset/entrySize), block.begin() + offset - (offset/entrySize) + returnAmount};
	}




	//testing:
	void printMemory() {
		for (const auto& [key, value] : memory) {
			// Print the key
			std::cout << "\nMemory lines: " << std::endl;
			std::cout << "Key " << key << ": [";

			// Print each element of the vector separated by "|"
			bool first = true;
			for (const auto& element : value) {
				if (first) {
					first = false;
				} else {
					std::cout << " | ";
				}
				std::cout << element;
			}
			std::cout << "]" <<std::endl;
		}
	}
	//test end

};

struct Cache {
	typedef uint32_t Address;
	typedef uint32_t Tag;
	typedef uint32_t Index;
	typedef uint32_t Offset;

	struct CacheLine {
		bool occupied = false;
		Tag tag = 0;
		std::vector<uint32_t> line;

		CacheLine(uint32_t size): line(size, 0){}
	};

	unsigned cacheLines;
	unsigned cacheLineSize;
	std::vector<CacheLine> cache;
	uint32_t entrySize;
	int offsetBitAmount;
	int indexBitAmount;
	uint32_t hits = 0;
	uint32_t misses = 0;
	uint32_t cycles = 0;

	Memory memory;

	Cache(unsigned cacheLines, unsigned cacheLineSize, int entrySize):
		cacheLines(cacheLines),
		cacheLineSize(cacheLineSize),
		cache(cacheLines, CacheLine(cacheLineSize/entrySize)),
		entrySize(entrySize),
		offsetBitAmount(static_cast<int>(std::ceil(std::log2(cacheLineSize)))),
		indexBitAmount(static_cast<int>(std::ceil(std::log2(cacheLines)))),
		memory(cacheLineSize, cacheLineSize/entrySize, entrySize)

	{}

	void insert_read(Request request) {
		//extract Tag index and offset bits
		Offset offset = request.addr << (32-offsetBitAmount);
		offset >>= (32-offsetBitAmount);

		Index index = request.addr << (32-offsetBitAmount-indexBitAmount);
		index >>= (32-indexBitAmount);

		Tag tag = request.addr >> (offsetBitAmount+indexBitAmount);


		if (request.we) {
			if (cache[index].occupied) {
				if (cache[index].tag == tag) {
					memory.insert(request);
					cache[index].line[offset/entrySize] = request.data;
					hits++;
				} else {
					cache[index].tag = tag;
					cache[index].line = memory.insert(request);
					misses++;
				}
			} else {
				cache[index].tag = tag;
				cache[index].occupied = true;
				cache[index].line = memory.insert(request);
				misses++;
			}
		} else {
			if (cache[index].occupied) {
				if (cache[index].tag == tag) {
					hits++;
				} else {
					cache[index].tag = tag;
					cache[index].line = memory.insert(request);
					misses++;
				}
			} else {
				cache[index].tag = tag;
				cache[index].occupied = true;
				cache[index].line = memory.insert(request);
				misses++;
			}
			std::cout << "\nValue for address " << request.addr << ": " << cache[index].line[offset/entrySize] << std::endl;
			cycles++;
		}


		//Testing:
		std::bitset<32> Tag(tag);
		std::bitset<32> Index(index);
		std::bitset<32> Offset(offset);
		std::bitset<32> Address(request.addr);

		std::cout << "\nCacheStuff: " << std::endl;
		std::cout << "	Address binary: " << Address << " | Decimal: " << request.addr << std::endl;
		std::cout << "	Tag: " << Tag << " | Decimal: " << tag << std::endl;
		std::cout << "	Index: " << Index << " | Decimal: " << index << std::endl;
		std::cout << "	Offset: " << Offset << " | Decimal: " << offset << std::endl;
		std::cout << "\nCache lines:" << std::endl;
		printCache();
		std::cout << "Misses: " << misses  << " | Hits: " << hits << std::endl;
		//Test end

	}

	//Testing:
	void printCache() {
		for (CacheLine cacheLine: cache) {
			std::cout << "Tag " << cacheLine.tag << ": [";
			for (uint32_t entry : cacheLine.line) {
				std::cout << " " << entry << ",";
			}
			std::cout<< "]" << std::endl;
		}
	}
	//Test end

};


int sc_main(int argc, char* argv[]) {
	std::cout << "ERROR" << std::endl;
	return 0;
}