//
// Created by Nicolas von Mallinckrodt on 24.06.24.
//

#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include "systemc.h"
using namespace sc_core;

extern "C" {

    struct Request {
        uint32_t addr;
        uint32_t data;
        int we; // 0 for read, 1 for write
    };

    struct Result {
        size_t cycles;
        size_t misses;
        size_t hits;
        size_t primitiveGateCount;
    };

    struct Result run_simulation(
            int cycles,
            int directMapped, // 0 for full-associative, 1 for direct mapped
            unsigned cacheLines,
            unsigned cacheLineSize,
            unsigned cacheLatency,
            unsigned memoryLatency,
            size_t numRequests,
            Request* requests[numRequests],
            const char* traceFile
            );
}

SC_MODULE(CacheLine) {
    //struct for cache line, which include tag, index, and offset bits

};

SC_MODULE(Cache) {
    sc_in<bool> clk;

    //inputs
    sc_in<sc_vector<uint32_t>> addr;
    sc_in<sc_vector<uint32_t>> data_in; //for write

    //outputs
    sc_out<sc_vector<uint32_t>> data_out; //for read
    sc_out<bool> hits; // 0 for miss, 1 for hit
    

    void process() {

    }
};


#endif
