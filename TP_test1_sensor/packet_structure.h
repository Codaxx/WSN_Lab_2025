#ifndef PACKET_STRUCTURES_H
#define PACKET_STRUCTURES_H
/*******************STRUCTURES**********************/
// This structure is used routing table entries.
// Total byte = 8 byte.
typedef struct rt_entry{
	uint8_t type;// Standard C includes:
	linkaddr_t dest;
	linkaddr_t next_hop;
	uint8_t tot_hop;		// Total hop number for this destination.
	int16_t metric;
	uint16_t seq_no;
	// used for constructiong the local routing table
	struct rt_entry *next;
}rt_entry;

typedef struct{

	uint8_t type;// Standard C includes:
	uint8_t no_entries;
	rt_entry table[MAX_NODES];

}routing_table;

typedef struct{
	uint8_t type;// Standard C includes:
	linkaddr_t src;
	uint8_t no_entries;
	rt_entry table[MAX_NODES]; // battery status
}node_info;

struct dao_packet {
  	uint8_t type;// Standard C includes:
	linkaddr_t src;
	uint8_t no_entries;
	rt_entry table[MAX_NODES]; // battery status
};


//the packet used for intial the set-up process
struct dio_packet {
  uint8_t type;
  linkaddr_t src_master;                 // original sender (Master node)
  uint8_t hop_count;             // current hop count from master
  uint16_t seq_id;               // sequence ID to prevent loops
};





/********************ROUTING LIST*************************/


#endif
