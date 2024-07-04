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
	std::vector<cacheLine> cache;
	int offset;
	int index;


	Cache(unsigned cacheLines, unsigned cacheLineSize):
		cacheLines(cacheLines), cacheLineSize(cacheLineSize), cache(cacheLines, cacheLine(cacheLineSize)), index(log2(cacheLines))
	{

	}

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
	//construct main memory
	//hashmap for the areas in memory
	//memory stores an ordered map of the block with values

	//Methods:
		// Write

		//Read
	unsigned cacheLines;
	unsigned cacheLineSize;
	unsigned cacheLatency;
	unsigned memoryLatency;

	Cache sampleCache(20, 4);
	std::cout << sampleCache.cacheLines << std::endl;
	std::cout << sampleCache.cache[0].line.size() << std::endl;
	sampleCache.cache[0].line[0] = 2;
	sampleCache.printCache();





	return 0;
    //std::cout << "ERROR" << std::endl;
    //return 1;
}