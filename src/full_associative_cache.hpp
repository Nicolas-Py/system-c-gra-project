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

    /*
     Since full associative caches do not have index bits, we can't directly index to the cache lines, therefore we have to search through the whole cache line by line
     and compare the tags to find the correct cache line, if we do not find the line by its, we find an empty line or we select a random line to replace if all cache lines are full.
     */
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
            /*
             For full associative cache, the address only contains the tag bits and offset bits.
             The tag bits and offset bits are extracted from the address using shift operations.
             */
            MEMORY_REQUEST req = request.read();
            Offset offset = req.addr << (32 - offsetBitAmount);
            offset >>= (32 - offsetBitAmount);
            Tag data_tag = req.addr >> offsetBitAmount;

            //this line does not mean that we are indexing with a full associative cache, we are finding the line using the findLine function identified by its tag.
            int index = findLine(data_tag);

            //Write operation
            if (req.we) {
                if (cache[index].occupied) {
                    if (cache[index].tag == data_tag) {
                        update_memory_request.write(req); //memory write back
                        wait(main_memory.blockUpdated);

                        cache[index].line[offset/entrySize] = req.data;
                        out.write(1);
                    } else {
                        update_memory_request.write(req); //memory write back
                        wait(main_memory.blockUpdated);

                        cache[index].tag = data_tag;
                        cache[index].line = memory_block.read().block;
                        out.write(0);
                    }
                } else {
                    update_memory_request.write(req); //memory write back
                    wait(main_memory.blockUpdated);

                    cache[index].occupied = true;
                    cache[index].tag = data_tag;
                    cache[index].line = memory_block.read().block;
                    out.write(0);
                }
            //for reading operation
            } else {
                if (cache[index].occupied) {
                    if (cache[index].tag == data_tag) {
                        out.write(1);
                    } else {
                        update_memory_request.write(req); //memory write back
                        wait(main_memory.blockUpdated);

                        cache[index].tag = data_tag;
                        cache[index].line = memory_block.read().block;
                        out.write(0);
                    }
                } else {
                    update_memory_request.write(req); //memory write back
                    wait(main_memory.blockUpdated);

                    cache[index].occupied = true;
                    cache[index].tag = data_tag;
                    cache[index].line = memory_block.read().block;
                    out.write(0);
                }
            }
            wait();
        }
    }
};

#endif //FULL_ASSOCIATIVE_CACHE_HPP
