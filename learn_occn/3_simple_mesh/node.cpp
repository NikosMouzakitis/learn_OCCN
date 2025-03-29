#include "node.h"
#include <iostream>

Node::Node(sc_module_name name, uint32_t x, uint32_t y) : 
    sc_module(name),
    x_pos(x),
    y_pos(y)
{
    SC_HAS_PROCESS(Node);
    SC_THREAD(send_packets);
    SC_THREAD(receive_packets);
    std::cout << "Created " << name << " at (" << x << "," << y << ")\n";
}

void Node::send_packets() {
    sc_time timeout(100, SC_NS);
    if(x_pos == 0 && y_pos == 0) {
        while(true) {
            MeshPacket* pkt = new MeshPacket;
            static int packet_num = 0;

            MESH_HDR(*pkt, source_x) = 0;
            MESH_HDR(*pkt, source_y) = 0;
            MESH_HDR(*pkt, dest_x) = 1;
            MESH_HDR(*pkt, dest_y) = 1;
            MESH_HDR(*pkt, sequence) = packet_num++;

            for(int i=0; i<56; i++) {
                MESH_BODY(*pkt, i) = 'A' + (i % 26);
            }

            std::cout << "\n" << sc_time_stamp() << " [Node" << x_pos << y_pos << "] ATTEMPTING TO SEND PACKET:\n"
                      << "  Source: (" << MESH_HDR(*pkt, source_x) << "," << MESH_HDR(*pkt, source_y) << ")\n"
                      << "  Final Destination: (" << MESH_HDR(*pkt, dest_x) << "," << MESH_HDR(*pkt, dest_y) << ")\n"
                      << "  Next Hop: (1,0) via east port\n"
                      << "  Sequence: " << MESH_HDR(*pkt, sequence) << "\n";
            
            bool sent;
            east_out.send(pkt, timeout, sent);
            
            if(!sent) {
                std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] SEND FAILED! Could not send to Node(1,0)\n";
                delete pkt;
            } else {
                std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] Packet successfully sent to Node(1,0)\n";
            }

            wait(200, SC_NS);
        }
    }
}

void Node::receive_packets() {
    while(true) {
        bool received;
        sc_time timeout(100, SC_NS);
        MeshPacket* pkt = nullptr;

        // Check all input ports with proper if-else syntax
        if((pkt = east_in.receive(timeout, received))) {
            std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] Received packet from west (Node" 
                      << (x_pos-1) << y_pos << ")\n";
        } 
        else if((pkt = west_in.receive(timeout, received))) {
            std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] Received packet from east (Node" 
                      << (x_pos+1) << y_pos << ")\n";
        } 
        else if((pkt = north_in.receive(timeout, received))) {
            std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] Received packet from south (Node" 
                      << x_pos << (y_pos-1) << ")\n";
        } 
        else if((pkt = south_in.receive(timeout, received))) {
            std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] Received packet from north (Node" 
                      << x_pos << (y_pos+1) << ")\n";
        }

        if(received && pkt) {
            std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] PACKET DETAILS:\n"
                      << "  Source: (" << MESH_HDR(*pkt, source_x) << "," << MESH_HDR(*pkt, source_y) << ")\n"
                      << "  Final Destination: (" << MESH_HDR(*pkt, dest_x) << "," << MESH_HDR(*pkt, dest_y) << ")\n"
                      << "  Sequence: " << MESH_HDR(*pkt, sequence) << "\n";

            uint32_t dest_x = MESH_HDR(*pkt, dest_x);
            uint32_t dest_y = MESH_HDR(*pkt, dest_y);

            if(dest_x == x_pos && dest_y == y_pos) {
                std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] PACKET REACHED FINAL DESTINATION!\n";
                delete pkt;
            } else {
                forward_packet(pkt);
            }
        }
        wait(SC_ZERO_TIME);
    }
}

void Node::forward_packet(MeshPacket* pkt) {
    uint32_t dest_x = MESH_HDR(*pkt, dest_x);
    uint32_t dest_y = MESH_HDR(*pkt, dest_y);
    bool sent = false;
    sc_time timeout(50, SC_NS);
    std::string next_node;

    if(dest_x != x_pos) { // X-direction first
        if(dest_x > x_pos) {
            east_out.send(pkt, timeout, sent);
            next_node = "Node" + std::to_string(x_pos+1) + std::to_string(y_pos);
        } else {
            west_out.send(pkt, timeout, sent);
            next_node = "Node" + std::to_string(x_pos-1) + std::to_string(y_pos);
        }
    } else { // Y-direction
        if(dest_y > y_pos) {
            north_out.send(pkt, timeout, sent);
            next_node = "Node" + std::to_string(x_pos) + std::to_string(y_pos+1);
        } else {
            south_out.send(pkt, timeout, sent);
            next_node = "Node" + std::to_string(x_pos) + std::to_string(y_pos-1);
        }
    }

    if(!sent) {
        std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] FORWARDING FAILED! Could not send to " << next_node << "\n";
        delete pkt;
    } else {
        std::cout << sc_time_stamp() << " [Node" << x_pos << y_pos << "] Forwarded packet to " << next_node << "\n";
    }
}
