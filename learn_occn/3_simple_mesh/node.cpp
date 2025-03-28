#include "node.h"
#include <iostream>

Node::Node(sc_module_name name, uint32_t x, uint32_t y) :
	sc_module(name), x_pos(x), y_pos(y)
{
	std::cout << "creating node " << name << std::endl;
	SC_THREAD(send_packets);
	SC_THREAD(receive_packets);
}

void Node::send_packets() {
	int packet_num = 0;

	while(true) {

		if(strcmp(this->name(), "Node_0_0") == 0) {

			cout << this->name() << " attempt to sent message" << endl;

			//uint32_t dest_x = (x_pos + 1) % 2;
			//uint32_t dest_y = (y_pos + 1) % 2;
			uint32_t dest_x = 1;
			uint32_t dest_y = 1;

			MeshPacket* pkt = new MeshPacket;

			// Use correct header access
			MESH_HDR(*pkt, source_x) = x_pos;
			MESH_HDR(*pkt, source_y) = y_pos;
			MESH_HDR(*pkt, dest_x) = dest_x;
			MESH_HDR(*pkt, dest_y) = dest_y;
			MESH_HDR(*pkt, sequence) = packet_num++;

			// Fill payload
			for(int i=0; i<56; i++) {
				MESH_BODY(*pkt, i) = 'A' + (i % 26);
			}

			cout << sc_time_stamp() << " Node(" << x_pos << "," << y_pos
			     << "): Sending packet " <<  packet_num << " to node ("<< dest_x<<","<<dest_y<<")" << endl;

			bool sent;
			sc_time timeout(100, SC_NS);
			to_net.send(pkt, timeout, sent);

			if(!sent) {
				cout << this->name() << " Failed to send packet!" << endl;
				delete pkt;
			}
			cout << this->name() << "pkt sent" << endl;

			wait(200, SC_NS);
		} else {
			wait();
		}
	}
}

void Node::receive_packets() {
	sc_time timeout(100, SC_NS); // Create named timeout object

	while(true) {
		bool received;
		MeshPacket* pkt = from_net.receive(timeout, received);

		if(received) {
			cout << sc_time_stamp() << " Node(" << x_pos << "," << y_pos
			     << "): Received packet from ("
			     << MESH_HDR(*pkt, source_x) << "," << MESH_HDR(*pkt, source_y)
			     << "), seq=" << MESH_HDR(*pkt, sequence) << endl;

			//print payload
			for(int i = 0 ; i < 1; i++)
			{
				char recb = MESH_BODY(*pkt,i);
				std::cout << this->name() <<  " received data: " << recb << std::endl;
			}

			from_net.reply();
			delete pkt;
		}
	}
}
