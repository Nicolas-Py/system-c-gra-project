//
// Created by Daniel Singh on 15.07.24.
//

#ifndef MAIN_MEMORY_HPP
#define MAIN_MEMORY_HPP

#include <unordered_map>
#include "modules.hpp"

//own block definition to be able to use sc_signal<Block>
struct Block {
    std::vector<uint32_t> block;

    Block(): block(0, 0) {}

    Block(uint32_t size): block(size, 0) {}

    //overwritten the assignment operator
    Block& operator=(const Block& block2) {
        if (this != &block2) {
            block = block2.block;
        }
        return *this;
    }

    //overitten the equality check opertor
    bool operator==(const Block& block2) const {
        return block == block2.block;
    }
};

// defined output for printing for example for std::cout on a sc_signal<Block>
inline std::ostream& operator<<(std::ostream& output, const Block& block) {
    output << "Block: [";
    for (int i = 0; i < block.block.size(); i++) {
        output << block.block[i];
        if (i < block.block.size() - 1) {
            output << " | ";
        }
    }
    output << "]";
    return output;
}

//defined how sc_trace should trace an sc_signal<Block>
inline void sc_trace(sc_trace_file* file, const Block& block, const std::string& name) {
    for (int i = 0; i < block.block.size(); i++) {
        sc_trace(file, block.block[i], name + ".value=" + std::to_string(i) + "");
    }
}

SC_MODULE(MAIN_MEMORY) {
    typedef uint32_t Address;
    typedef uint32_t Tag;
    typedef uint32_t Index;
    typedef uint32_t Offset;

    uint32_t cacheLineSize;
    uint32_t blockAmount; // amount of values a cache line can store
    uint32_t offsetBitAmount;
    uint32_t entrySize; // size of a cache entry

    /*
    our memory will consist of a hashmap which stores the Tag-Index bits as key and as value
    contains an vector which can be directly index with the offset
    */
    std::unordered_map<uint32_t, std::vector<uint32_t>> memory;
    Block returnBlock;

    sc_in<MEMORY_REQUEST> request;
    sc_out<Block> out;


    SC_CTOR(MAIN_MEMORY);
    MAIN_MEMORY(sc_module_name name, uint32_t cacheLineSize, uint32_t blockAmount, uint32_t entrySize):
        sc_module(name),
        cacheLineSize(cacheLineSize),
        blockAmount(blockAmount),
        offsetBitAmount(static_cast<int>(std::ceil(std::log2(cacheLineSize)))),
        entrySize(entrySize),
        returnBlock(blockAmount)
    {
        SC_THREAD(update);
        sensitive << request;
        dont_initialize();
    }

    void update() {
        while(true) {

            //extract tag-index bits for key in hashmap and offset for index in the vector
            Address tag_index = request.read().addr >> offsetBitAmount;
            Offset offset = request.read().addr << (32-offsetBitAmount);
            offset >>= (32-offsetBitAmount);

            //Testing:
            std::bitset<32> TAG_INDEX(tag_index);
            std::bitset<32> Offset(offset);
            std:: cout << "\nMemory stuff: " << std::endl;
            std::cout << "	Tag_Index: " << TAG_INDEX << " | Decimal: " << tag_index << std::endl;
            std::cout << "	Offset: " << Offset << " | Decimal: " << offset << std::endl;
            //test end

            //if tag-index is not yet found in the memory create a vector for addresses of that combination
            if (memory.find(tag_index) == memory.end()) {
                memory[tag_index] = std::vector<uint32_t>(cacheLineSize, 0);
            }

            //write through functunality in case of a write request
            if (request.read().we) {
                memory[tag_index][offset] = request.read().data;
            }

            //block to replace the cacheline with
            returnBlock.block = extractBlock(offset, tag_index);

            printMemory();
            out.write(returnBlock);
            wait();
        }
    }

    //functunality to return then required block from the main memory to the cacheline for replacement
    std::vector<uint32_t> extractBlock(Offset offset, Address tag_index) {
        std::vector<uint32_t>& block = memory[tag_index];
        return {block.begin() + offset - (offset/entrySize), block.begin() + offset - (offset/entrySize) + blockAmount};
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

#endif //MAIN_MEMORY_HPP
