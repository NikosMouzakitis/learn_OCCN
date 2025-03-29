#include "node.h"
#include <occn.h>
#include <sstream>

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
    srand(time(NULL));

    sc_clock clock("clk", 10, SC_NS);
    Node* nodes[2][2];
    
    // Create nodes
    for(int y=0; y<2; y++) {
        for(int x=0; x<2; x++) {
            std::stringstream name;
            name << "Node_" << x << "_" << y;
            nodes[x][y] = new Node(name.str().c_str(), x, y);
        }
    }

    // Create and connect ALL channels
    occn::StdChannel<MeshPacket, MeshPacket>* channels[2][2][4]; // [x][y][direction]

    // Initialize all channels
    for(int y=0; y<2; y++) {
        for(int x=0; x<2; x++) {
            for(int d=0; d<4; d++) {
                std::stringstream chan_name;
                chan_name << "chan_" << x << y << "_" << d;
                channels[x][y][d] = new occn::StdChannel<MeshPacket, MeshPacket>(chan_name.str().c_str());
                channels[x][y][d]->clk(clock);
            }
        }
    }

    // ================== CONNECT ALL PORTS ================== //
    // Node(0,0) connections
    nodes[0][0]->east_out(*channels[0][0][0]);  // East ->
    nodes[1][0]->west_in(*channels[0][0][0]);   // <-
    nodes[0][0]->west_out(*channels[0][0][1]);  // West ->
    nodes[1][0]->east_in(*channels[0][0][1]);   // <-
    nodes[0][0]->north_out(*channels[0][0][2]); // North ->
    nodes[0][1]->south_in(*channels[0][0][2]);  // <-
    nodes[0][0]->south_out(*channels[0][0][3]); // South ->
    nodes[0][1]->north_in(*channels[0][0][3]);  // <-

    // Node(1,0) connections
    nodes[1][0]->east_out(*channels[1][0][0]);  // East ->
    nodes[0][0]->west_in(*channels[1][0][0]);   // <-
    nodes[1][0]->west_out(*channels[1][0][1]);  // West ->
    nodes[0][0]->east_in(*channels[1][0][1]);   // <-
    nodes[1][0]->north_out(*channels[1][0][2]); // North ->
    nodes[1][1]->south_in(*channels[1][0][2]);  // <-
    nodes[1][0]->south_out(*channels[1][0][3]); // South ->
    nodes[1][1]->north_in(*channels[1][0][3]);  // <-

    // Node(0,1) connections
    nodes[0][1]->east_out(*channels[0][1][0]);  // East ->
    nodes[1][1]->west_in(*channels[0][1][0]);   // <-
    nodes[0][1]->west_out(*channels[0][1][1]);  // West ->
    nodes[1][1]->east_in(*channels[0][1][1]);   // <-
    nodes[0][1]->north_out(*channels[0][1][2]); // North ->
    nodes[0][0]->south_in(*channels[0][1][2]);  // <-
    nodes[0][1]->south_out(*channels[0][1][3]); // South ->
    nodes[0][0]->north_in(*channels[0][1][3]);  // <-

    // Node(1,1) connections
    nodes[1][1]->east_out(*channels[1][1][0]);  // East ->
    nodes[0][1]->west_in(*channels[1][1][0]);   // <-
    nodes[1][1]->west_out(*channels[1][1][1]);  // West ->
    nodes[0][1]->east_in(*channels[1][1][1]);   // <-
    nodes[1][1]->north_out(*channels[1][1][2]); // North ->
    nodes[1][0]->south_in(*channels[1][1][2]);  // <-
    nodes[1][1]->south_out(*channels[1][1][3]); // South ->
    nodes[1][0]->north_in(*channels[1][1][3]);  // <-

    // Verify all ports are bound
    for(int y=0; y<2; y++) {
        for(int x=0; x<2; x++) {
            if(!nodes[x][y]->east_out.get_interface() || 
               !nodes[x][y]->east_in.get_interface() ||
               !nodes[x][y]->west_out.get_interface() ||
               !nodes[x][y]->west_in.get_interface() ||
               !nodes[x][y]->north_out.get_interface() ||
               !nodes[x][y]->north_in.get_interface() ||
               !nodes[x][y]->south_out.get_interface() ||
               !nodes[x][y]->south_in.get_interface()) {
                SC_REPORT_ERROR("Binding", "Unbound ports detected!");
            }
        }
    }

    std::cout << "All ports successfully bound!\n";
    sc_start(1000, SC_NS);

    // Cleanup
    for(int y=0; y<2; y++) {
        for(int x=0; x<2; x++) {
            delete nodes[x][y];
            for(int d=0; d<4; d++) {
                delete channels[x][y][d];
            }
        }
    }

    return 0;
}
