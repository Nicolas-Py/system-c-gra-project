//
// Created by Daniel Singh on 15.07.24.
//

#ifndef DIRECT_MAPPED_CACHE_HPP
#define DIRECT_MAPPED_CACHE_HPP

#include "modules.hpp"
#include "main-memory.hpp"

SC_MODULE(DIRECT_MAPPED_CACHE) {
    typedef uint32_t Address;
    typedef uint32_t Tag;
    typedef uint32_t Index;
    typedef uint32_t Offset;

    /*
    each cacheline is going to be of this type with a vector holding
    the data to the corresponding tag
    */
    struct CacheLine {
        bool occupied = false;
        Tag tag = 0;
        std::vector<uint32_t> line;

        CacheLine(uint32_t size): line(size, 0){}
    };

    unsigned cacheLines; //cache line amount
    unsigned cacheLineSize; //cacheline size in byte
    uint32_t entrySize; //size of each entry
    std::vector<CacheLine> cache; //cache holding each cacheLine

    int offsetBitAmount;
    int indexBitAmount;

    sc_in<MEMORY_REQUEST> request;
    sc_out<int> out; //hit or miss

    //signals for main memory binding
    MAIN_MEMORY main_memory;
    sc_signal<MEMORY_REQUEST> mainMemoryRequest; //signal to send the request to main memory
    sc_signal<Block> memoryBlock; //signal to hold the retuned block from main memory


    SC_CTOR(DIRECT_MAPPED_CACHE);
    DIRECT_MAPPED_CACHE(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, int entrySize):
        sc_module(name),
        cacheLines(cacheLines),
        cacheLineSize(cacheLineSize),
        cache(cacheLines, CacheLine(cacheLineSize/entrySize)), // initialises each line to hold cacheLineSize/entrySize entries
        entrySize(entrySize),
        offsetBitAmount(static_cast<int>(std::ceil(std::log2(cacheLineSize)))),
        indexBitAmount(static_cast<int>(std::ceil(std::log2(cacheLines)))),
        main_memory("main_memory", cacheLineSize, cacheLineSize/entrySize, entrySize)
    {

        //memory bindings
        main_memory.request.bind(mainMemoryRequest);
        main_memory.out.bind(memoryBlock);


        SC_THREAD(update);
        sensitive << request;
        dont_initialize();
    }

    void update() {
        while (true) {
            //logic to extract offset, index and tag bits
            Offset offset = request.read().addr << (32-offsetBitAmount);
            offset >>= (32-offsetBitAmount);

            Index index = request.read().addr << (32-offsetBitAmount-indexBitAmount);
            index >>= (32-indexBitAmount);

            Tag tag = request.read().addr >> (offsetBitAmount+indexBitAmount);

           if (request.read().we) {
               if (cache[index].occupied) {
                    if (cache[index].tag == tag) {
                        //update memory and wait for the block to be present
                        mainMemoryRequest.write(request.read());
                        wait(main_memory.blockUpdated);

                        cache[index].line[offset/entrySize] = request.read().data;
                        out.write(1);
                    } else {
                        //update memory and wait for the block to be present
                        mainMemoryRequest.write(request.read());
                        wait(main_memory.blockUpdated);

                        cache[index].tag = tag;
                        cache[index].line = memoryBlock.read().block;
                        out.write(0);
                    }
               } else {
                   //update memory and wait for the block to be present
                   mainMemoryRequest.write(request.read());
                   wait(main_memory.blockUpdated);

                   cache[index].tag = tag;
                   cache[index].occupied = true;
                   cache[index].line = memoryBlock.read().block;
                   out.write(0);
               }
           } else {
               if (cache[index].occupied) {
                   if (cache[index].tag == tag) {
                       out.write(1);
                   } else {
                       //update memory and wait for the block to be present
                       mainMemoryRequest.write(request.read());
                       wait(main_memory.blockUpdated);

                       cache[index].tag = tag;
                       cache[index].line = memoryBlock.read().block;
                       out.write(0);
                   }
               } else {
                   //update memory and wait for the block to be present
                   mainMemoryRequest.write(request.read());
                   wait(main_memory.blockUpdated);

                   cache[index].tag = tag;
                   cache[index].occupied = true;
                   cache[index].line = memoryBlock.read().block;
                   out.write(0);
               }
           }

            wait();
        }
    }
};

#endif //DIRECT_MAPPED_CACHE_HPP