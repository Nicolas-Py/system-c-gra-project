//
// Created by Daniel Singh on 15.07.24.
//

#ifndef MAIN_MEMORY_HPP
#define MAIN_MEMORY_HPP

#include "modules.hpp"

SC_MODULE(MAIN_MEMORY) {
    sc_signal<bool> a;
    sc_signal<bool> b;

    SC_CTOR(MAIN_MEMORY) {
        SC_THREAD(behaviour);
        sensitive << a << b;
        dont_initialize();
    }

    void behaviour() {
        while(true) {
            std::cout << (a.read() & b.read()) << std::endl;
            wait();
        }
    }
};

#endif //MAIN_MEMORY_HPP
