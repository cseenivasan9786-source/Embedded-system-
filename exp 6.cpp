#include "design.h"

int sc_main(int argc, char* argv[]) {
    ProductionLine line("line");
    Station assembly("assembly_station", false);
    Station qc("quality_station", true);
    line.assembly.bind(assembly.socket);
    line.inspection.bind(qc.socket);
    std::cout << "Experiment 6: TLM Production Line Simulator\n";
    sc_start(50, SC_NS);
    return (line.completed && line.passed == 4 && line.rejected == 1) ? 0 : 1;
}
