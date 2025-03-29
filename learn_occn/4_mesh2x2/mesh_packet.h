#ifndef MESH_PACKET_H
#define MESH_PACKET_H

#include <occn.h>
#include "systemc.h"

// Packet Types
enum MeshPacketType {
	DATA_PACKET,
	CONTROL_PACKET,
	MEMORY_REQUEST,
	MEMORY_RESPONSE,
	TASK_PACKET
};

struct MeshHeader {
	// Routing information
	uint32_t source_x;    // X-coordinate of source node
	uint32_t source_y;    // Y-coordinate of source node
	uint32_t dest_x;      // X-coordinate of destination node
	uint32_t dest_y;      // Y-coordinate of destination node

	// Packet identification
	uint32_t sequence;    // Sequence number
	MeshPacketType type;  // Packet type

	// Quality of Service
	uint32_t timestamp;   // Creation time
	uint8_t priority;     // Priority level (0-255)

	// Error checking
	uint16_t checksum;    // Optional checksum
};

// Packet Body (56 bytes total)
struct MeshPayload {
	union {
		// For DATA_PACKET
		struct {
			uint32_t address;
			uint32_t data[13];  // 52 bytes of data
		} memory_op;

		// For TASK_PACKET
		struct {
			uint32_t task_id;
			uint32_t parameters[13];
		} task;

		// Raw data access
		uint8_t raw[56];
	} data;

	// Add ostream operator
	friend std::ostream& operator<<(std::ostream& os, const MeshPayload& payload) {
		os << "[Payload]";
		return os;
	}
};

// Define the complete packet using OCCN PDU
typedef occn::Pdu<MeshHeader, MeshPayload, 1> MeshPacket;

// Access macros (updated for safety)
#define MESH_HDR(pkt, field) ((pkt).view_as.pdu.hdr.field)
#define MESH_BODY(pkt)       ((pkt).view_as.pdu.body[0])

// Helper functions
inline void init_packet(MeshPacket& pkt, uint32_t src_x, uint32_t src_y, uint32_t dest_x, uint32_t dest_y, MeshPacketType type = DATA_PACKET) {
	MESH_HDR(pkt, source_x) = src_x;
	MESH_HDR(pkt, source_y) = src_y;
	MESH_HDR(pkt, dest_x) = dest_x;
	MESH_HDR(pkt, dest_y) = dest_y;
	MESH_HDR(pkt, type) = type;
	MESH_HDR(pkt, timestamp) = (uint32_t)(sc_time_stamp().to_default_time_units());
	MESH_HDR(pkt, sequence) = 0;
	MESH_HDR(pkt, priority) = 0;
	MESH_HDR(pkt, checksum) = 0;
	// Initialize payload to 0
	memset(&MESH_BODY(pkt), 0, sizeof(MeshPayload));
}

inline void print_packet(const MeshPacket& pkt) {
	std::cout << "Packet [" << MESH_HDR(pkt, sequence) << "]:\n"
	          << "  Type: " << MESH_HDR(pkt, type) << "\n"
	          << "  From: (" << MESH_HDR(pkt, source_x)
	          << "," << MESH_HDR(pkt, source_y) << ")\n"
	          << "  To: (" << MESH_HDR(pkt, dest_x)
	          << "," << MESH_HDR(pkt, dest_y) << ")\n"
	          << "  Timestamp: " << MESH_HDR(pkt, timestamp) << "\n";
}

#endif // MESH_PACKET_H
