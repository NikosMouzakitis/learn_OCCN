#ifndef MESH_PACKET
#define MESH_PACKET
#include <occn.h>

struct MeshHeader {
    uint32_t source_x;
    uint32_t source_y; 
    uint32_t dest_x;
    uint32_t dest_y;
    uint32_t sequence;
};

// Use OCCN's header access macros
typedef occn::Pdu<MeshHeader, char, 56> MeshPacket;
#define MESH_HDR(pkt, field) (pkt).view_as.pdu.hdr.field
#define MESH_BODY(pkt, idx)  (pkt).view_as.pdu.body[idx]

#endif
