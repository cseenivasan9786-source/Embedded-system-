#ifndef DESIGN_H
#define DESIGN_H
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <iostream>
using namespace sc_core;
using namespace tlm;

SC_MODULE(Station) {
    tlm_utils::simple_target_socket<Station> socket;
    bool quality;
    SC_HAS_PROCESS(Station);
    Station(sc_module_name name, bool qc)
        : sc_module(name), socket("socket"), quality(qc) {
        socket.register_b_transport(this, &Station::transport);
    }

    void transport(tlm_generic_payload& tx, sc_time& delay) {
        if (tx.get_command() != TLM_WRITE_COMMAND) {
            tx.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
            return;
        }
        if (!tx.get_data_ptr() || tx.get_data_length() != 1) {
            tx.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
            return;
        }
        // Assembly accepts every item. QC rejects item 3 for demonstration.
        tx.get_data_ptr()[0] = quality && tx.get_address() == 3 ? 0 : 1;
        delay += sc_time(quality ? 3 : 4, SC_NS);
        tx.set_response_status(TLM_OK_RESPONSE);
    }
};

SC_MODULE(ProductionLine) {
    tlm_utils::simple_initiator_socket<ProductionLine> assembly;
    tlm_utils::simple_initiator_socket<ProductionLine> inspection;
    unsigned passed = 0, rejected = 0;
    bool completed = false;

    bool send(tlm_utils::simple_initiator_socket<ProductionLine>& port,
              unsigned item, unsigned char& result) {
        tlm_generic_payload tx;
        sc_time delay = SC_ZERO_TIME;
        tx.set_command(TLM_WRITE_COMMAND);
        tx.set_address(item);
        tx.set_data_ptr(&result);
        tx.set_data_length(1);
        tx.set_streaming_width(1);
        tx.set_byte_enable_ptr(nullptr);
        tx.set_dmi_allowed(false);
        tx.set_response_status(TLM_INCOMPLETE_RESPONSE);
        port->b_transport(tx, delay);
        wait(delay); // Consume annotated transaction latency
        if (tx.is_response_error()) {
            SC_REPORT_ERROR("TLM", "Transaction failed");
            return false;
        }
        return true;
    }

    void run() {
        for (unsigned item = 1; item <= 5; ++item) {
            unsigned char result = 0;
            if (!send(assembly, item, result)) return;
            std::cout << sc_time_stamp() << " | Item " << item
                      << " assembled\n";
            if (!send(inspection, item, result)) return;
            if (result) ++passed; else ++rejected;
            std::cout << sc_time_stamp() << " | Item " << item
                      << (result ? " QC PASS\n" : " QC FAIL\n");
            if (item < 5) wait(2, SC_NS);
        }
        std::cout << "Total: 5 | Passed: " << passed
                  << " | Rejected: " << rejected << '\n';
        completed = true;
    }

    SC_CTOR(ProductionLine) : assembly("assembly"), inspection("inspection") {
        SC_THREAD(run);
    }
};
#endif
