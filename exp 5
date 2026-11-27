#ifndef DESIGN_H
#define DESIGN_H
#include <systemc>
#include <iostream>
using namespace sc_core;

SC_MODULE(ConcurrentProcesses) {
    sc_in<bool> clk;
    unsigned produced = 0;
    unsigned consumed = 0;

    void producer() {
        while (true) {
            std::cout << sc_time_stamp() << " | Producer: item "
                      << ++produced << " created\n";
            wait(); // Wait for next positive clock edge
        }
    }

    void consumer() {
        while (true) {
            if (consumed < produced) {
                std::cout << sc_time_stamp() << " | Consumer: item "
                          << ++consumed << " processed\n";
            }
            wait(); // Wait for next negative clock edge
        }
    }

    SC_CTOR(ConcurrentProcesses) {
        SC_THREAD(producer);
        sensitive << clk.pos();
        dont_initialize();
        SC_THREAD(consumer);
        sensitive << clk.neg();
        dont_initialize();
    }
};
#endif
