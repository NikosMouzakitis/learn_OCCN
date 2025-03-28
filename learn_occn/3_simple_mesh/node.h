#ifndef NODE_H
#define NODE_H

#include "mesh_packet.h"

class Node : public sc_module {
public:
    occn::MasterPort<MeshPacket> to_net;
    occn::SlavePort<MeshPacket> from_net;
    
    const uint32_t x_pos, y_pos;
    
    SC_CTOR(Node);
    Node(sc_module_name name, uint32_t x, uint32_t y);
    
    void send_packets();
    void receive_packets();
};

#endif
