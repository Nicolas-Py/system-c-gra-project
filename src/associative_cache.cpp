//
// Created by Ziang Liu on 15.07.24.
//

#include <unordered_map>
#include "associative_cache.hpp"

#include "modules.hpp"

Result run_simulation (
    int cycles,
    int directMapped,
    unsigned cacheLines,
    unsigned cacheLineSize,
    unsigned cacheLatency,
    unsigned memoryLatency,
    size_t numRequests,
    Request requests[numRequests],
    const char* tracefile
) {
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
        std:: cout << "\nMemory : " << std::endl;
        std::cout << "	Tag_Index: " << TAG_INDEX << " | Decimal: " << tag_index << std::endl;
        std::cout << "	Offset: " << Offset << " | Decimal: " << offset << std::endl;

        //test end


        if (memory.find(tag_index) == memory.end()) {
            memory[tag_index] = std::vector<uint32_t>(cacheLineSize, 0);
        }
        if (request.we) {
            memory[tag_index][offset] = request.data;
        }

        //printMemory();
        return extractBlock(offset, tag_index);
    }

    std::vector<uint32_t> extractBlock(Offset offset, Address tag_index) {
        std::vector<uint32_t>& block = memory[tag_index];
        return {block.begin() + offset - (offset/entrySize), block.begin() + offset - (offset/entrySize) + returnAmount};
    }


    /*
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
    */
    //test end


};

struct Associative_Cache {
    typedef uint32_t Address;
    typedef uint32_t Tag;
    typedef uint32_t Offset;

    struct Cache_Line {
        bool occupied = false;
        Tag tag = 0;
        std::vector<uint32_t> line;
        uint32_t lastAccess = 0;

        Cache_Line(uint32_t size): line(size, 0) {}
    };

    unsigned cacheLines;
    unsigned cacheLineSize;
    std::vector<Cache_Line> cache;
    uint32_t entrySize;
    int offsetBitAmount;
    uint32_t hits = 0;
    uint32_t misses = 0;
    uint32_t cycles = 0;

    Memory memory;

    Associative_Cache(unsigned cacheLines, unsigned cacheLineSize, int entrySize):
    cacheLines(cacheLines),
    cacheLineSize(cacheLineSize),
    cache(cacheLines, Cache_Line(cacheLineSize / entrySize)),
    entrySize(entrySize),
    offsetBitAmount(static_cast<int>(std::ceil(std::log2(cacheLineSize)))),
    memory(cacheLineSize, cacheLineSize / entrySize, entrySize) {}

    void insert_read(Request request) {
        Offset offset = request.addr << (32 - offsetBitAmount);
        offset >>= (32 - offsetBitAmount);

        Tag data_tag = request.addr >> offsetBitAmount;

        Cache_Line* line = nullptr;

        /*
         Associative caches do not have index bits in the address, they only have tag bits and offset bits.
         Since we do not have index bits, we would have to identify the cache line by their tag bits, with worst case of O(cacheLine).
         First, I created a cache line object referenced to a null pointer.
         */

        /*
         Given the necessary tag from the address, I would proceed to look for the cache line specific to the tag.
         This for-loop deals with the scenario when we want to read/write from/to the cache. It will successfully find the cache line under two conditions
         when the cache line's tag is equal to our tag extracted from the request address and obviously the cache line is not empty.
         We will not find the cache line if the whole cache is empty or when the cache is full but none of the cache line tags are equal to our tag.
         We deal with this in the next for-loop.
         */
        for (auto& cache_line : cache) {
            if (cache_line.occupied && cache_line.tag == data_tag) {
                line = &cache_line;
                break;
            }
        }


        /*
         Under the condition that we have not found our cache line for the purpose of read/write. We deal with it accordingly:
         We would find a cache line that is unoccupied, so we can write into it, or when we have a read miss, we would fetch the tag specifc data from
         the memory and put it into the unoccupied cache line.

         When the cache is full and and want to insert a new data into the cache and do not know which cache line to replace, we would find the cache line
         with the smallest last access count, which is the LRU policy.
         */
        if (!line) {
            line = &cache[0];
            for (auto& cache_line : cache) {
                if (!cache_line.occupied) {
                    line = &cache_line;
                    break;
                }
                if (cache_line.lastAccess < line->lastAccess) {
                    line = &cache_line;
                }
            }
        }


        /*
        In the following, the memory.insert(Request) serves as a write through to the main memory and extract a data block from the memory.

        If the tags are equal, we are only updating one part of the cache line and write through to the memory using
            1. memory.insert(Request)
            2. line->line[offset/entrySize] = request.data

        If tags are not equal, we will update the memory and return the data back to the cache line
            1. line->line = memory.insert(Request)

        !! Although both of the options do the same operation, it is not the same logic and they do not have the same execution time.
        */
        if (request.we) {
            if (line->occupied) {
                if (line->tag == data_tag) {
                    memory.insert(request);
                    line->line[offset/entrySize] = request.data;
                    hits++;
                } else {
                    line->tag = data_tag;
                    line->line = memory.insert(request);
                    misses++;
                }
            } else {
                line->occupied = true;
                line->tag = data_tag;
                line->line = memory.insert(request);
                misses++;
            }
        } else {
            if (line->occupied) {
                if (data_tag == line->tag) {
                    hits++;
                } else {
                    line->tag = data_tag;
                    line->line = memory.insert(request);
                    misses++;
                }
            } else {
                line->tag = data_tag;
                line->line = memory.insert(request);
                misses++;
            }
            std::cout << "\nValue for address " << request.addr << ": " << line->line[offset/entrySize] << std::endl;
            cycles++;
        }
        line->lastAccess = cycles;




        //for testing
        std::bitset<32> TagBits(data_tag);
        std::bitset<32> OffsetBits(offset);
        std::bitset<32> AddressBits(request.addr);

        std::cout << "\nCacheStuff: " << std::endl;
        std::cout << "    Address binary: " << AddressBits << " | Decimal: " << request.addr << std::endl;
        std::cout << "    Tag: " << TagBits << " | Decimal: " << data_tag << std::endl;
        std::cout << "    Offset: " << OffsetBits << " | Decimal: " << offset << std::endl;
        std::cout << "\nCache lines:" << std::endl;
        printCache();
        std::cout << "Misses: " << misses << " | Hits: " << hits << std::endl;


    }

    void printCache() {
        for (const auto& cacheLine: cache) {
            std::cout << "Tag " << cacheLine.tag << ": [";
            for (uint32_t entry : cacheLine.line) {
                std::cout << " " << entry << ",";
             }
            std::cout << "]" << std::endl;
        }
    }


};

int sc_main(int argc, char* argv[]) {

    unsigned cacheLine = 8;
    unsigned cacheLineSize = 32;
    int entrySize = 4;

    Associative_Cache associative_cache(cacheLine, cacheLineSize, entrySize);

    std::cout << "Cache line size: " << associative_cache.cacheLineSize << ", Offset bits: " << associative_cache.offsetBitAmount << std::endl;
    std::cout << "Number of cache lines: " << associative_cache.cacheLines << std::endl;
    std::cout << "Entry size: " << associative_cache.entrySize << std::endl;

    Request requests[] = {
        {0xABCD, 0x1234, 1},
        {0xABCD, 0, 0}
    };

    for (auto request : requests) {
        associative_cache.insert_read(request);
    }


    return 0;
}

