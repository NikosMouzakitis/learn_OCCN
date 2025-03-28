#include "node.h"
#include <occn.h>
#include <nlohmann/json.hpp> 

using json = nlohmann::json;

int sc_main(int argc, char* argv[]) {
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

	// Seed random number generator
	srand(time(NULL));

	sc_clock clock("clk", 10, SC_NS);
	
	// Create a JSON object to store NoC data
	json noc_data;
	noc_data["topology"] = {
		{"type", "2D Mesh"},
		{"rows", 2},
		{"cols", 2}
	};
	// Create the nodes of the 2x2 mesh.
	
	Node* nodes[2][2];
	for(int y=0; y<2; y++) {
		for(int x=0; x<2; x++) {
			std::string name = "Node_" + std::to_string(x) + "_" + std::to_string(y);
			nodes[x][y] = new Node(name.c_str(), x, y);

			// Log node info
			noc_data["nodes"][name] = {
				{"x", x},
				{"y", y},
				{"connections", {}}
			};
		}
	}

	// Create channels among the nodes
	occn::StdChannel<MeshPacket, MeshPacket>* channels[2][2][2]; // [x][y][direction]

	// Horizontal connections
	channels[0][0][0] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_00_10");
	channels[1][0][0] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_10_00");
	channels[0][1][0] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_01_11");
	channels[1][1][0] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_11_01");

	// Vertical connections
	channels[0][0][1] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_00_01");
	channels[0][1][1] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_01_00");
	channels[1][0][1] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_10_11");
	channels[1][1][1] = new occn::StdChannel<MeshPacket, MeshPacket>("chan_11_10");
	
	// Log connections
	json connections;
	connections["00_10"] = {{"from", "Node_0_0"}, {"to", "Node_1_0"}, {"direction", "east"}};
	connections["10_00"] = {{"from", "Node_1_0"}, {"to", "Node_0_0"}, {"direction", "west"}};
	connections["01_11"] = {{"from", "Node_0_1"}, {"to", "Node_1_1"}, {"direction", "east"}};
	connections["11_01"] = {{"from", "Node_1_1"}, {"to", "Node_0_1"}, {"direction", "west"}};
	connections["00_01"] = {{"from", "Node_0_0"}, {"to", "Node_0_1"}, {"direction", "north"}};
	connections["01_00"] = {{"from", "Node_0_1"}, {"to", "Node_0_0"}, {"direction", "south"}};
	connections["10_11"] = {{"from", "Node_1_0"}, {"to", "Node_1_1"}, {"direction", "north"}};
	connections["11_10"] = {{"from", "Node_1_1"}, {"to", "Node_1_0"}, {"direction", "south"}};
	noc_data["connections"] = connections;



	// Connect nodes (simplified routing - each node connects to two others)

	// Node(0,0) connections
	nodes[0][0]->to_net(*channels[0][0][0]);  // East
	channels[0][0][0]->clk(clock);
	nodes[1][0]->from_net(*channels[0][0][0]);

	nodes[0][0]->to_net(*channels[0][0][1]);  // North
	channels[0][0][1]->clk(clock);
	nodes[0][1]->from_net(*channels[0][0][1]);

	// Node(0,1) connections
	nodes[0][1]->to_net(*channels[0][1][0]);  // East
	channels[0][1][0]->clk(clock);
	nodes[1][1]->from_net(*channels[0][1][0]);

	nodes[0][1]->to_net(*channels[0][1][1]);  // North
	channels[0][1][1]->clk(clock);
	nodes[0][0]->from_net(*channels[0][1][1]);

	// Node(1,0) connections
	nodes[1][0]->to_net(*channels[1][0][0]);  // East
	channels[1][0][0]->clk(clock);
	nodes[0][0]->from_net(*channels[1][0][0]);

	nodes[1][0]->to_net(*channels[1][0][1]);  // North
	channels[1][0][1]->clk(clock);
	nodes[1][1]->from_net(*channels[1][0][1]);

	// Node(1,1) connections
	nodes[1][1]->to_net(*channels[1][1][0]);  // East
	channels[1][1][0]->clk(clock);
	nodes[0][1]->from_net(*channels[1][1][0]);

	nodes[1][1]->to_net(*channels[1][1][1]);  // North
	channels[1][1][1]->clk(clock);
	nodes[1][0]->from_net(*channels[1][1][1]);
	
		// Write JSON to file
	std::ofstream out("noc_topology.json");
	out << std::setw(4) << noc_data << std::endl;
	out.close();


	cout << "Starting simplified 2x2 Mesh NoC simulation..." << endl;
	sc_start(1000, SC_NS);

	return 0;
}

