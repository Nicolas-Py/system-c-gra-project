//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//
#include "modules.hpp"
#include "direct-mapped-cache.hpp"
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
	Result testResult = {1, 1, 1, 1};
    return testResult;
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

	/*
	unsigned cacheLines = 8;
	unsigned cacheLineSize = 32;
	unsigned cacheLatency;
	unsigned memoryLatency;


	Cache sampleCache(cacheLines, cacheLineSize, 4);

	std::cout << "\nCache stuff:" << std::endl;
	std::cout << "	CacheLines: " << sampleCache.cacheLines << " | IndexBitAmount: " << sampleCache.indexBitAmount << std::endl;
	std::cout << "	CacheLineSize: " << sampleCache.cacheLineSize << " | OffsetBitAmount " << sampleCache.offsetBitAmount << std::endl;
	std::cout << "	EntrySize: " << sampleCache.entrySize << std::endl;

	Request requests[] = {
		{0, 42, 0},        // Read
		{4, 56, 1},        // Write
		{8, 72, 0},        // Read
		{12, 15, 1},       // Write
		{16, 99, 0},       // Read
		{20, 3, 1},        // Write
		{24, 88, 0},       // Read
		{28, 45, 1},       // Write
		{32, 67, 0},       // Read
		{36, 128, 1},      // Write
		{40, 256, 0},      // Read
		{44, 512, 1},      // Write
		{48, 1024, 0},     // Read
		{52, 2048, 1},     // Write
		{56, 4096, 0},     // Read
		{60, 8192, 1},     // Write
		{64, 16384, 0},    // Read
		{68, 32768, 1},    // Write
		{72, 65536, 0},    // Read
		{76, 131072, 1},   // Write
		{80, 262144, 0},   // Read
		{84, 524288, 1},   // Write
		{88, 1048576, 0},  // Read
		{92, 2097152, 1},  // Write
		{96, 4194304, 0},  // Read
		{100, 8388608, 1}, // Write
		{104, 16777216, 0},// Read
		{108, 33554432, 1},// Write
		{112, 67108864, 0},// Read
		{116, 134217728, 1}// Write
	};

	for (auto request: requests) {
		sampleCache.insert_read(request);
	}


	std::cout << "hits: " << sampleCache.hits << std::endl;
	std::cout << "misses: " << sampleCache.misses << std::endl;
	*/

	MAIN_MEMORY main_memory("Main_memory");

	sc_start();

	
	return 0;

	return 0;
    //std::cout << "ERROR" << std::endl;
    //return 1;
}