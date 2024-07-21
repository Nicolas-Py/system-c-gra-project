//
// Created by Ziang Liu on 18.07.24.
//

#ifndef FULL_ASSOCIATIVE_CACHE_HPP
#define FULL_ASSOCIATIVE_CACHE_HPP

#include "main-memory.hpp"
#include "modules.hpp"
#include <random>


SC_MODULE(ASSOCIATIVE_CACHE) {
    typedef uint32_t Address;
    typedef uint32_t Offset;
    typedef uint32_t Tag;

    struct CacheLine {
        bool occupied = false;
        Tag tag = 0;
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

    int findLine(Tag data_tag) {
        for (int i = 0; i < cache.size(); i++) {
            if (cache[i].occupied && cache[i].tag == data_tag) {
                return i;
            }
        }
        for (int i = 0; i < cache.size(); i++) {
            if (!cache[i].occupied) {
                return i;
            }
        }
        //stack overflow question:
        //https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, cache.size() - 1);
        return dist(rng);
    }


    void update() {
        while (true) {
            MEMORY_REQUEST req = request.read();
            Offset offset = req.addr << (32 - offsetBitAmount);
            offset >>= (32 - offsetBitAmount);

            Tag data_tag = req.addr >> offsetBitAmount;

            int index = findLine(data_tag);

            if (req.we) {
                if (cache[index].occupied) {
                    if (cache[index].tag == data_tag) {
                        update_memory_request.write(req);
                        wait(main_memory.blockUpdated);

                        cache[index].line[offset/entrySize] = req.data;
                        out.write(1);
                    } else {
                        update_memory_request.write(req);
                        wait(main_memory.blockUpdated);

                        cache[index].tag = data_tag;
                        cache[index].line = memory_block.read().block;
                        out.write(0);
                    }
                } else {
                    update_memory_request.write(req);
                    wait(main_memory.blockUpdated);

                    cache[index].occupied = true;
                    cache[index].tag = data_tag;
                    cache[index].line = memory_block.read().block;
                    out.write(0);
                }
            } else {
                if (cache[index].occupied) {
                    if (cache[index].tag == data_tag) {
                        out.write(1);
                    } else {
                        update_memory_request.write(req);
                        wait(main_memory.blockUpdated);

                        cache[index].tag = data_tag;
                        cache[index].line = memory_block.read().block;
                        out.write(0);
                    }
                } else {
                    update_memory_request.write(req);
                    wait(main_memory.blockUpdated);

                    cache[index].occupied = true;
                    cache[index].tag = data_tag;
                    cache[index].line = memory_block.read().block;
                    out.write(0);
                }
            }


            /*
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
            */

            wait();
        }
    }



    void printCache() {
        for (CacheLine cacheLine: cache) {
            std::cout << "Tag " << cacheLine.tag << ": [";
            for (uint32_t entry : cacheLine.line) {
                std::cout << " " << entry << ",";
            }
            std::cout<< "] " << std::endl;
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
