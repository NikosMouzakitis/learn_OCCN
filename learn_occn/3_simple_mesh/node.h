#ifndef NODE_H
#define NODE_H

#include "mesh_packet.h"
#include <occn.h>

class Node : public sc_module {
public:
    // Complete directional ports
    occn::MasterPort<MeshPacket> east_out;
    occn::SlavePort<MeshPacket> east_in;
    occn::MasterPort<MeshPacket> west_out;
    occn::SlavePort<MeshPacket> west_in;
    occn::MasterPort<MeshPacket> north_out;
    occn::SlavePort<MeshPacket> north_in;
    occn::MasterPort<MeshPacket> south_out;
    occn::SlavePort<MeshPacket> south_in;

    const uint32_t x_pos, y_pos;
    
    Node(sc_module_name name, uint32_t x, uint32_t y);
    
    void send_packets();
    void receive_packets();

private:
    void forward_packet(MeshPacket* pkt);
    sc_event send_trigger;
};
#endif
