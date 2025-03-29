#include "router.h"

Router::Router(sc_module_name name, int x, int y) :
	sc_module(name), x_pos(x), y_pos(y)
{
	east_out = NULL;
	east_in = NULL;
	west_in = NULL;
	west_out = NULL;
	north_in = NULL;
	north_out =  NULL;
	south_in = NULL;
	south_out = NULL;

	if(x_pos == 1)
	{
		west_in = new occn::SlavePort<MeshPacket>();
		west_out = new occn::MasterPort<MeshPacket>();
	}
	if(x_pos == 0) {
		east_in = new occn::SlavePort<MeshPacket>();
		east_out = new occn::MasterPort<MeshPacket>();
	}
	if(y_pos == 0) {
		north_in = new occn::SlavePort<MeshPacket>();
		north_out = new occn::MasterPort<MeshPacket>();
	}
	if(y_pos == 1) {
		south_in = new occn::SlavePort<MeshPacket>();
		south_out = new occn::MasterPort<MeshPacket>();
	}

	std::cout << this->name() << " constructor" << std::endl;
	SC_THREAD(route_packets);
//    SC_THREAD(process_pe_packets);
}

void Router::route_packets() {
	while(true) {
		sc_time timeout(50, SC_NS);
		bool received;

		// Try receiving from each port
		occn::Pdu<MeshHeader, MeshPayload, 1>* pkt = nullptr;

		// East
		if(east_in) {
			pkt = east_in->receive(timeout, received);
			if(received) {
				forward_packet(*pkt);
				continue;
			}
		}
		// West
		if(west_in) {
			pkt = west_in->receive(timeout, received);
			if(received) {
				forward_packet(*pkt);
				continue;
			}
		}
		// North
		if(north_in) {
			pkt = north_in->receive(timeout, received);
			if(received) {
				forward_packet(*pkt);
				continue;
			}
		}

		// South
		if(south_in) {
			pkt = south_in->receive(timeout, received);
			if(received) {
				forward_packet(*pkt);
				continue;
			}
		}
		wait(SC_ZERO_TIME);
	}
}

void Router::forward_packet(MeshPacket& pkt) {
	uint32_t dest_x = MESH_HDR(pkt, dest_x);
	uint32_t dest_y = MESH_HDR(pkt, dest_y);
	sc_time timeout(50, SC_NS);
	bool sent;

	cout << sc_time_stamp() << " [" << name() << "] Received packet for (" << dest_x << "," << dest_y << ")" << endl;

	// Local delivery
	if(dest_x == x_pos && dest_y == y_pos) {
		cout << "FINAL DESTINATION" << endl;
		//  pe_out.write(pkt);
		return;
	}

	// XY Routing
	if(dest_x != x_pos) {
		if( (dest_x > x_pos) && east_out) {
			east_out->send(&pkt, timeout, sent);
			if(sent) {
				cout << "  Sent EAST to (" << (x_pos+1) << "," << y_pos << ")" << endl;
				return;
			}
		} else if( (dest_x < x_pos) && west_out) {
			west_out->send(&pkt, timeout, sent);

			if(sent) {
				cout << "  Sent WEST to (" << (x_pos-1) << "," << y_pos << ")" << endl;
				return;
			}

		}
	}
	if(dest_y != y_pos) {
		if(dest_y > y_pos && north_out) {
			north_out->send(&pkt, timeout, sent);
			if(sent) {
				cout << "  Sent NORTH to (" << x_pos << "," << (y_pos+1) << ")" << endl;
				return;
			}
		}
		else if(dest_y < y_pos && south_out) {
			south_out->send(&pkt, timeout, sent);
			if(sent) {
				cout << "  Sent SOUTH to (" << x_pos << "," << (y_pos-1) << ")" << endl;
				return;
			}
		}
	}

	cerr << "ERROR: Couldn't route packet from (" << x_pos << "," << y_pos
	     << ") to (" << dest_x << "," << dest_y << ")" << endl;

}
/*
void Router::process_pe_packets() {
    while(true) {
        MeshPacket pkt;
        pe_in.read(pkt);

        MESH_HDR(pkt, source_x) = x_pos;
        MESH_HDR(pkt, source_y) = y_pos;

        forward_packet(pkt);
        wait(SC_ZERO_TIME);
    }
}
*/
Router::~Router() {
	// Delete west ports if they exist
	if(x_pos == 0) {
		delete west_in;
		delete west_out;
	}

	// Delete east ports if they exist
	if(x_pos == 1) {
		delete east_in;
		delete east_out;
	}

	// Delete north ports if they exist
	if(y_pos == 0) {
		delete north_in;
		delete north_out;
	}

	// Delete south ports if they exist
	if(y_pos == 1) {
		delete south_in;
		delete south_out;
	}

	std::cout << this->name() << " destructor" << std::endl;
}
