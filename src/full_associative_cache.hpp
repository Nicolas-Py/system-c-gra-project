//
// Created by Ziang Liu on 18.07.24.
//

#ifndef FULL_ASSOCIATIVE_CACHE_HPP
#define FULL_ASSOCIATIVE_CACHE_HPP

#include "main-memory.hpp"
#include "modules.hpp"


SC_MODULE(ASSOCIATIVE_CACHE) {
    typedef uint32_t Address;
    typedef uint32_t Offset;
    typedef uint32_t Tag;

    struct CacheLine {
        bool occupied = false;
        Tag tag = 0;
        uint32_t lastAccess = 0;
        std::vector<uint32_t> line;

        CacheLine (uint32_t size): line(size, 0) {}
    };

    unsigned cacheLines;
    unsigned cacheLineSize;

    uint32_t entrySize;
    std::vector<CacheLine> cache;

    int offsetBitAmount;
    sc_in<MEMORY_REQUEST> request;
    sc_out<int> out;
    int hits = 0, misses = 0;

    MAIN_MEMORY main_memory;
    sc_signal<MEMORY_REQUEST> update_memory_request;
    sc_signal<Block> memory_block;

    SC_CTOR(ASSOCIATIVE_CACHE);

    ASSOCIATIVE_CACHE(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, int entrySize):
    sc_module(name),
    cacheLines(cacheLines),
    cacheLineSize(cacheLineSize),
    cache(cacheLines, CacheLine(cacheLineSize/entrySize)),
    entrySize(entrySize),
    offsetBitAmount(static_cast<int>(std::ceil(std::log2(cacheLineSize)))),
    main_memory("main_memory", cacheLineSize, cacheLineSize/entrySize, entrySize)
    {
        main_memory.request.bind(update_memory_request);
        main_memory.out.bind(memory_block);

        SC_THREAD(update);
        sensitive << request;
        dont_initialize();
    }

    void update() {
        while (true) {
            MEMORY_REQUEST req = request.read();
            Offset offset = req.addr << (32 - offsetBitAmount);
            offset >>= (32 - offsetBitAmount);

            Tag data_tag = req.addr >> offsetBitAmount;

            CacheLine* line = nullptr;


            for (auto& cache_line : cache) {
                if (cache_line.occupied && cache_line.tag == data_tag) {
                    line = &cache_line;
                    break;
                }
            }

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

            if (req.we) {
                if (line->occupied) {
                    if (line->tag == data_tag) {
                        update_memory_request.write(req);
                        wait(main_memory.blockUpdated);

                        line->line[offset/entrySize] = req.data;
                        out.write(1);
                        hits++;
                    } else {
                        update_memory_request.write(req);
                        wait(main_memory.blockUpdated);

                        line->tag = data_tag;
                        line->line = memory_block.read().block;
                        out.write(0);
                        misses++;
                    }
                } else {
                    update_memory_request.write(req);
                    wait(main_memory.blockUpdated);

                    line->occupied = true;
                    line->tag = data_tag;
                    line->line = memory_block.read().block;
                    out.write(0);
                    misses++;
                }
            } else {
                if (line->occupied) {
                    if (line->tag == data_tag) {
                        hits++;
                        out.write(1);
                    } else {
                        update_memory_request.write(req);
                        wait(main_memory.blockUpdated);

                        line->tag = data_tag;
                        line->line = memory_block.read().block;
                        out.write(0);
                        misses++;
                    }
                } else {
                    update_memory_request.write(req);
                    wait(main_memory.blockUpdated);

                    line->occupied = true;
                    line->tag = data_tag;
                    line->line = memory_block.read().block;
                    out.write(0);
                    misses++;
                }
            }
            line->lastAccess = misses+hits;


            std::bitset<32> Tag(data_tag);
            std::bitset<32> Offset(offset);
            std::bitset<32> Address(request.read().addr);

            std::cout << "\nCacheStuff: " << std::endl;
            std::cout << "	Address binary: " << Address << " | Decimal: " << request.read().addr << std::endl;
            std::cout << "	Tag: " << Tag << " | Decimal: " << data_tag << std::endl;
            std::cout << "	Offset: " << Offset << " | Decimal: " << offset << std::endl;
            std::cout << "\nCache lines:" << std::endl;
            printCache();
            std::cout << "Misses: " << misses  << " | Hits: " << hits << std::endl;
            //Test end

            wait();
        }
    }



    void printCache() {
        for (CacheLine cacheLine: cache) {
            std::cout << "Tag " << cacheLine.tag << ": [";
            for (uint32_t entry : cacheLine.line) {
                std::cout << " " << entry << ",";
            }
            std::cout<< "] " << "LRU count: " << cacheLine.lastAccess << std::endl;
        }
    }

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





#endif //FULL_ASSOCIATIVE_CACHE_HPP
