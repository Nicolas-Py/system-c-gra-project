//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//
#include "modules.hpp"
#include <systemc>
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


struct Cache {
	typedef uint32_t Address;
	typedef uint32_t Tag;
	typedef uint32_t Index;
	typedef uint32_t Offset;

	struct cacheLine {
		bool occupied = false;
		Tag tag = 0;
		std::vector<uint32_t> line;

		cacheLine(size_t size): line(size, 0){}
	};

	unsigned cacheLines;
	unsigned cacheLineSize;
	std::vector<CacheLine> cache;
	uint32_t entrySize;
	int offsetBitAmount;
	int indexBitAmount;


	Cache(unsigned cacheLines, unsigned cacheLineSize, int entrySize):
		cacheLines(cacheLines),
		cacheLineSize(cacheLineSize),
		cache(cacheLines, CacheLine(cacheLineSize/entrySize)),
		entrySize(entrySize),
		offsetBitAmount(static_cast<int>(std::ceil(std::log2(cacheLineSize)))),
		indexBitAmount(static_cast<int>(std::ceil(std::log2(cacheLines))))
	{}

	void insert_read(Request request) {
		Offset offset = request.addr << (32-offsetBitAmount);
		offset >>= (32-offsetBitAmount);

		Index index = request.addr << (32-offsetBitAmount-indexBitAmount);
		index >>= (32-indexBitAmount);

		Index index = request.addr << (32-offsetBitAmount-indexBitAmount);
		index >>= (32-indexBitAmount);

		Tag tag = request.addr >> (offsetBitAmount+indexBitAmount);

		//Testing:
		std::bitset<32> Tag(tag);
		std::bitset<32> Index(index);
		std::bitset<32> Offset(offset);

		std::cout << "Tag: " << Tag << " | Decimal: " << tag << std::endl;
		std::cout << "Index: " << Index << " | Decimal: " << index << std::endl;
		std::cout << "Offset: " << Offset << " | Decimal: " << offset << std::endl;
		//Test end

		CacheLine& line = cache[index];
		if (request.we) {
			if (line.occupied) {
				if (line.tag == tag) {
					line.line[offset/entrySize] = request.data;
					//write through to memory
				} else {
					//fetch block from memory
					line.tag = tag;
					for (int i=0; i<cacheLineSize/entrySize; i++) {
						line.line[i] = -100;
					}
				}
			} else {
				line.occupied = true;
				line.tag = tag;
				line.line[offset/entrySize] = request.data;
			}
		} else {
			if (line.occupied) {
				if (line.tag == tag) {
					line.line[offset] = request.data;
				} else {
					//fetch block from memory
					line.tag = tag;
					for (int i=0; i<cacheLineSize/entrySize; i++) {
						line.line[i] = -100;
					}
				}
			}
		}

	}

	//Testing:
	void printCache() {
		for (cacheLine cacheLine: cache) {
			std::cout << "Tag " << cacheLine.tag << ": [";
			for (uint32_t entry : cacheLine.line) {
				std::cout << " " << entry << ",";
			}
			std::cout<< "]" << std::endl;
		}
	}

	void insert(Request request) {

	}

};


int sc_main(int argc, char* argv[]) {
	unsigned cacheLines = 4;
	unsigned cacheLineSize = 32;
	unsigned cacheLatency;
	unsigned memoryLatency;
	unsigned addressSize = 4;

	Request x = {0x0, 24, 1};

	Cache sampleCache(cacheLines, cacheLineSize, sizeof(x.data));

	std::cout << "CacheLines: " << sampleCache.cacheLines << " | IndexBitAmount: " << sampleCache.indexBitAmount << std::endl;
	std::cout << "CacheLineSize: " << sampleCache.cacheLineSize << " | OffsetBitAmount " << sampleCache.offsetBitAmount << std::endl;
	std::cout << "EntrySize: " << sampleCache.entrySize << std::endl;

	std::bitset<32> sample(x.addr);
	std::cout << "Address binary: " << sample << std::endl;
	sampleCache.insert_read(x);

	sampleCache.printCache();





	return 0;
    //std::cout << "ERROR" << std::endl;
    //return 1;
}