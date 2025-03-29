#ifndef ROUTER_H
#define ROUTER_H

#include "mesh_packet.h"
#include <occn.h>

class Router : public sc_module {
public:
	// OCCN Ports
	occn::MasterPort<MeshPacket> *east_out;
	occn::SlavePort<MeshPacket> *east_in;
	occn::MasterPort<MeshPacket> *west_out;
	occn::SlavePort<MeshPacket> *west_in;
	occn::MasterPort<MeshPacket> *north_out;
	occn::SlavePort<MeshPacket> *north_in;
	occn::MasterPort<MeshPacket> *south_out;
	occn::SlavePort<MeshPacket> *south_in;

	// PE Interface
//    sc_fifo<MeshPacket> pe_in;
//   sc_fifo<MeshPacket> pe_out;

	const int x_pos, y_pos;

	SC_HAS_PROCESS(Router);
	Router(sc_module_name name, int x, int y);
	~Router();
	void route_packets();
//   void process_pe_packets();

private:
	void forward_packet(MeshPacket& pkt);
};
#endif
