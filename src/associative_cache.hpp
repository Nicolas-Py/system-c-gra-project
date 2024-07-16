//
// Created by Ziang Liu on 15.07.24.
//

#ifndef ASSOCIATIVE_CACHE_H
#define ASSOCIATIVE_CACHE_H

#include <systemc>
#include "systemc.h"
#include "modules.hpp"

using namespace sc_core;





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

#endif //ASSOCIATIVE_CACHE_H
