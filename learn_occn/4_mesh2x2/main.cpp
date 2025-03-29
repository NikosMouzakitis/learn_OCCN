#include "router.h"



int sc_main(int argc, char* argv[]) {
	//disable warnings.
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);


	sc_clock clk("clk",10,SC_NS);

	// Create 2x2 routers
	Router* routers[2][2];
	for(int y=0; y<2; y++) {
		for(int x=0; x<2; x++) {
			std::string name = "Router_" + std::to_string(x) + "_" + std::to_string(y);
			routers[x][y] = new Router(name.c_str(), x, y);
		}
	}

	// Create ALL communication channels (including dummies)
	// Horizontal channels
	occn::StdChannel<MeshPacket>* h_00_10 = new occn::StdChannel<MeshPacket>("h_00_10");
	occn::StdChannel<MeshPacket>* h_10_00 = new occn::StdChannel<MeshPacket>("h_10_00");
	occn::StdChannel<MeshPacket>* h_01_11 = new occn::StdChannel<MeshPacket>("h_01_11");
	occn::StdChannel<MeshPacket>* h_11_01 = new occn::StdChannel<MeshPacket>("h_11_01");
	// Vertical channels
	occn::StdChannel<MeshPacket>* v_00_01 = new occn::StdChannel<MeshPacket>("v_00_01");
	occn::StdChannel<MeshPacket>* v_01_00 = new occn::StdChannel<MeshPacket>("v_01_00");
	occn::StdChannel<MeshPacket>* v_10_11 = new occn::StdChannel<MeshPacket>("v_10_11");
	occn::StdChannel<MeshPacket>* v_11_10 = new occn::StdChannel<MeshPacket>("v_11_10");

	cout << "connection of router channels" << endl;

	h_00_10->clk(clk);
	h_10_00->clk(clk);
	h_01_11->clk(clk);
	h_11_01->clk(clk);
	v_00_01->clk(clk);
	v_01_00->clk(clk);
	v_10_11->clk(clk);
	v_11_10->clk(clk);

	// Connect ACTIVE horizontal links
	routers[0][0]->east_out->bind(*h_00_10);
	routers[1][0]->west_in->bind(*h_00_10);
	routers[1][0]->west_out->bind(*h_10_00);
	routers[0][0]->east_in->bind(*h_10_00);

	routers[0][1]->east_out->bind(*h_01_11);
	routers[1][1]->west_in->bind(*h_01_11);
	routers[1][1]->west_out->bind(*h_11_01);
	routers[0][1]->east_in->bind(*h_11_01);

	// Connect ACTIVE vertical links
	routers[0][0]->north_out->bind(*v_00_01);
	routers[0][1]->south_in->bind(*v_00_01);
	routers[0][1]->south_out->bind(*v_01_00);
	routers[0][0]->north_in->bind(*v_01_00);

	routers[1][0]->north_out->bind(*v_10_11);
	routers[1][1]->south_in->bind(*v_10_11);
	routers[1][1]->south_out->bind(*v_11_10);
	routers[1][0]->north_in->bind(*v_11_10);


	// Start simulation
	cout << "--------------------------------------------" << endl;
	cout << "-------Start 2x2 Mesh simulation------------" << endl;
	cout << "--------------------------------------------" << endl;
	sc_start(100, SC_NS);

	// Cleanup
	delete h_00_10;
	delete h_10_00;
	delete h_01_11;
	delete h_11_01;
	delete v_00_01;
	delete v_01_00;
	delete v_10_11;
	delete v_11_10;

	for(int x=0; x<2; x++) {
		for(int y=0; y<2; y++) {
			delete routers[x][y];
		}
	}

	return 0;
}
