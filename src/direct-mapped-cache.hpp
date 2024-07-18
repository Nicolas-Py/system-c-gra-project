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
    int hits, misses;

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
        main_memory.out(memoryBlock);


        SC_THREAD(update);
        sensitive << request;
        dont_initialize();
    }

    void update() {
        while (true) {
            //logic to offset, index and tag bits
            Offset offset = request.read().addr << (32-offsetBitAmount);
            offset >>= (32-offsetBitAmount);

            Index index = request.read().addr << (32-offsetBitAmount-indexBitAmount);
            index >>= (32-indexBitAmount);

            Tag tag = request.read().addr >> (offsetBitAmount+indexBitAmount);

           if (request.read().we) {
               if (cache[index].occupied) {
                    if (cache[index].tag == tag) {
                        //update memory wait for the block to be present
                        mainMemoryRequest.write(request.read());
                        wait(main_memory.blockUpdated);

                        cache[index].line[offset/entrySize] = request.read().data;
                        out.write(1);
                        hits++;
                    } else {
                        //update memory wait for the block to be present
                        mainMemoryRequest.write(request.read());
                        wait(main_memory.blockUpdated);

                        cache[index].tag = tag;
                        cache[index].line = memoryBlock.read().block;
                        out.write(0);
                        misses++;
                    }
               } else {
                   //update memory wait for the block to be present
                   mainMemoryRequest.write(request.read());
                   wait(main_memory.blockUpdated);

                   cache[index].tag = tag;
                   cache[index].occupied = true;
                   cache[index].line = memoryBlock.read().block;
                   out.write(0);
                   misses++;
               }
           } else {
               if (cache[index].occupied) {
                   if (cache[index].tag == tag) {
                       out.write(1);
                       hits++;
                   } else {
                       //update memory wait for the block to be present
                       mainMemoryRequest.write(request.read());
                       wait(main_memory.blockUpdated);

                       cache[index].tag = tag;
                       cache[index].line = memoryBlock.read().block;
                       out.write(0);
                       misses++;
                   }
               } else {
                   //update memory wait for the block to be present
                   mainMemoryRequest.write(request.read());
                   wait(main_memory.blockUpdated);

                   cache[index].tag = tag;
                   cache[index].occupied = true;
                   cache[index].line = memoryBlock.read().block;
                   out.write(0);

               }
           }


            //Testing:
            /*
            std::bitset<32> Tag(tag);
            std::bitset<32> Index(index);
            std::bitset<32> Offset(offset);
            std::bitset<32> Address(request.read().addr);

            std::cout << "\nCacheStuff: " << std::endl;
            std::cout << "	Address binary: " << Address << " | Decimal: " << request.read().addr << std::endl;
            std::cout << "	Tag: " << Tag << " | Decimal: " << tag << std::endl;
            std::cout << "	Index: " << Index << " | Decimal: " << index << std::endl;
            std::cout << "	Offset: " << Offset << " | Decimal: " << offset << std::endl;
            std::cout << "\nCache lines:" << std::endl;
            printCache();
            std::cout << "Misses: " << misses  << " | Hits: " << hits << std::endl;
            */
            //Test end

            wait();
        }
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

SC_MODULE(SOMETHING) {
    sc_signal<bool> a;

    DIRECT_MAPPED_CACHE direct;

    sc_signal<MEMORY_REQUEST> req;

    SC_CTOR(SOMETHING): direct("direct", 1, 2, sizeof(uint32_t)) {
        direct.request(req);

        SC_THREAD(update);
        sensitive << a;
        dont_initialize();
    }

    void update() {
        for (int i=0; i<10; i++) {
            MEMORY_REQUEST request(1, 2, 3, i);
            req.write(request);
            wait(SC_ZERO_TIME);
        }
    }
};


#endif //DIRECT_MAPPED_CACHE_HPP