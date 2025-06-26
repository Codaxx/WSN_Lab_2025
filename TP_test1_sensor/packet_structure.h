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
	routing_table current_routing_table; // route table
	rt_entry table[MAX_NODES]; // battery status
  uint8_t energy_level;
}node_info;

struct node_reply_packet {
  uint8_t    type;                
  linkaddr_t node_addr; //src link addr      
  uint8_t    hop_count;
  node_info  current_node_info;
};


//the packet used for intial the set-up process
struct hello_packet {
  uint8_t type;
  linkaddr_t originator;         // original sender (Master node)
  uint8_t hop_count;             // current hop count from master
  uint16_t seq_id;               // sequence ID to prevent loops
};





/********************ROUTING LIST*************************/


#endif
