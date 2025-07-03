#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "dev/leds.h"
#include "net/linkaddr.h"
#include "sys/node-id.h"
#include "sys/log.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "net/linkaddr.h"
#include <string.h>
#include <stdio.h>

#include "packet_structure.h"
#include "project-conf.h"

// used for recording the packet seq, prevent loop
static uint16_t last_seq_id = 0;
static linkaddr_t addr_master;
int8_t adjacency_matrix[MAX_NODES][MAX_NODES];  
static linkaddr_t node_index_to_addr[MAX_NODES]; 
static int num_known_nodes = 0;

LIST(local_rt_table);
MEMB(rt_mem,rt_entry,MAX_NODES);


uint16_t get_node_id_from_linkaddr(const linkaddr_t *addr) {
  return ((uint16_t)addr->u8[LINKADDR_SIZE - 2] << 8) | addr->u8[LINKADDR_SIZE - 1];
}


void print_adjacency_matrix()
{
  printf("Adjacency Matrix:\n   ");
  for (int j = 0; j < num_known_nodes; j++) {
    //print as node id 
    printf("%u     ",get_node_id_from_linkaddr(&node_index_to_addr[j]));
  }
  printf("\n");

  for (int i = 0; i < num_known_nodes; i++) {
    printf("%u",get_node_id_from_linkaddr(&node_index_to_addr[i]));
    for (int j = 0; j < num_known_nodes; j++) {
      if (adjacency_matrix[i][j] == -1)
        printf("  -  ");
      else
        printf("%3d  ", adjacency_matrix[i][j]);
    }
    printf("\n");
  }
}


int get_index_from_addr(const linkaddr_t *addr)
{
  for (int i = 0; i < num_known_nodes; i++) {
    if (linkaddr_cmp(addr, &node_index_to_addr[i])) {
      return i;
    }
  }
  // 不存在，尝试添加
  if (num_known_nodes < MAX_NODES) {
    linkaddr_copy(&node_index_to_addr[num_known_nodes], addr);
    return num_known_nodes++;
  }
  // 超出最大节点数量
  return -1;
}

const linkaddr_t *get_next_hop_to(const linkaddr_t *dest)
{
  for (rt_entry *e = list_head(local_rt_table); e != NULL; e = e->next) {
    if (linkaddr_cmp(&e->dest, dest)) {
      return &e->next_hop;
    }
  }
  return NULL;  // 找不到路径
}


void print_local_routing_table() {
  LOG_INFO("+------+ Local Routing Table: +------+\n");
  int i = 0;
  for(rt_entry *e = list_head(local_rt_table); e != NULL; e = e->next) {
    uint16_t dest_id = get_node_id_from_linkaddr(&e->dest);
    uint16_t next_id = get_node_id_from_linkaddr(&e->next_hop);
    LOG_INFO("|No.%d | dest:%u | next:%u | tot_hop:%u | rssi:%d | seq:%u |\n",
            i++, dest_id, next_id,
            e->tot_hop, e->metric, e->seq_no);
  }
  LOG_INFO("+------+----------------------+------+\n");
}
  
// Forward packet via broadcast
void forward_hello(struct hello_packet *pkt)
{
  nullnet_buf = (uint8_t *)pkt;
  nullnet_len = sizeof(*pkt);
  NETSTACK_NETWORK.output(NULL);
}

void update_local_rt_table(struct hello_packet *pkt)
{

}

bool parent_is_in_rt_table(const linkaddr_t *src)
{
  rt_entry *iter = list_head(local_rt_table);
  for(; iter != NULL; iter = iter->next) {
    if(linkaddr_cmp(&iter->dest, src))
    {
      return 1;
    }
    else;
  }
  return 0;
}

// Reply to hello packet
static void routing_report(const linkaddr_t *dest, uint8_t hop, int8_t rssi)
{
  // adding the local routing table info to the packet
  static struct rt_report_packet pkt;
  pkt.type = RT_REPORT_PACKET;
  linkaddr_copy(&pkt.src,&linkaddr_node_addr);
  //writing the local routing table to the packet
  pkt.no_entries = 0;
  rt_entry *iter = list_head(local_rt_table);
  for(; iter != NULL; iter = iter->next) {
    //pkt.table[no_entries++]=iter;
    pkt.table[pkt.no_entries].type     = iter->type;
    linkaddr_copy(&pkt.table[pkt.no_entries].dest,     &iter->dest);
    linkaddr_copy(&pkt.table[pkt.no_entries].next_hop, &iter->next_hop);
    pkt.table[pkt.no_entries].tot_hop = iter->tot_hop;
    pkt.table[pkt.no_entries].metric  = iter->metric;
    pkt.table[pkt.no_entries].seq_no  = iter->seq_no;
    pkt.no_entries++;
  }
  LOG_INFO("I have sent RT_REPORT_PACKET\n");
  // unicast to the parent node
  nullnet_buf = (uint8_t *)&pkt;
  //nullnet_len = sizeof(pkt);
  nullnet_len = offsetof(struct rt_report_packet, table) + pkt.no_entries * sizeof(rt_entry);
  NETSTACK_NETWORK.output(dest); 
}

// Receive hello packet callback
// 1.forward hello packet
// 2.reply to the src node
static void HELLO_PACKET_callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  //LOG_INFO("Hello Packet Process begin\r\n");
  leds_single_on(LEDS_LED2);
  // processing the hello packet info
  struct hello_packet *pkt = (struct hello_packet *)data;
  linkaddr_copy(&addr_master, &pkt->src_master);

  // Avoid loops: if already seen, drop
  if((pkt->seq_id <= last_seq_id) && !parent_is_in_rt_table(src)){
    leds_single_off(LEDS_LED2);
    //LOG_INFO("The Packet has been Processed\r\n");
    return;
  }

  int8_t rssi = (int8_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  // Update sequence tracking
  last_seq_id = pkt->seq_id;
  // Increment hop count
  pkt->hop_count++;
  // initialization ip_table, and add flooding info
  // update_local_rt_table(master_node info + hello packet info);
  // flooding connectivity
  rt_entry *e = memb_alloc(&rt_mem);
  if(e != NULL) {
    linkaddr_copy(&e->dest, src);
    linkaddr_copy(&e->next_hop, src);
    e->tot_hop = pkt->hop_count;
    e->metric = rssi;
    e->seq_no = pkt->seq_id;
    //uint16_t src_rt = get_node_id_from_linkaddr(&e->dest);
    //uint16_t next_rt = get_node_id_from_linkaddr(&e->next_hop);
    //LOG_INFO("Routing entry dest %i | next hop%i| tol_hop %u | rssi %i | seq_id %u\r\n",
    //     src_rt,next_rt,e->tot_hop,e->metric,e->seq_no);
    list_add(local_rt_table, e);  
  }
  // master node
  if (!linkaddr_cmp(src, &pkt->src_master))
  {
    rt_entry *e = memb_alloc(&rt_mem);
    if(e != NULL) {
      linkaddr_copy(&e->dest, &pkt->src_master);
      linkaddr_copy(&e->next_hop, src);
      e->tot_hop = pkt->hop_count;
      e->metric = rssi;
      e->seq_no = pkt->seq_id;
      list_add(local_rt_table, e);  
    }
  }

  // print the local_rt_table
  LOG_INFO("Local routing tabel is listed as follw: \r\n");
  print_local_routing_table();
  // Forward the packet
  forward_hello(pkt);
  // Reply the source
  routing_report(src, pkt->hop_count, rssi);

  leds_single_off(LEDS_LED2);
}

static void RT_REPORT_PACKET_callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  leds_single_on(LEDS_LED2);
  LOG_INFO("Receiving RT_REPORT_PACEKT:\n");
  const struct rt_report_packet *pkt = (const struct rt_report_packet *)data;
  if(node_id == MASTER_NODE_ID)
  {  
    LOG_INFO("Master Node get RT_REPORT_PACKET:\n");
    int src_index = get_index_from_addr(&pkt->src);
    if (src_index == -1) 
    { 
      LOG_WARN("Can't find the correct idx\n");
      return;
    }

    // update the adjacency matrix
    for (int i = 0; i < pkt->no_entries; i++) {
    const rt_entry *e = &pkt->table[i];
    int dst_index = get_index_from_addr(&e->dest);
    if (dst_index == -1) continue;
    adjacency_matrix[src_index][dst_index] = e->metric;
    }
    // update local_rt_table, now just simple adding the new entry
    for (int i = 0; i < pkt->no_entries; i++) {
      rt_entry *new_entry = memb_alloc(&rt_mem);
      const rt_entry *recv_entry = &pkt->table[i];
      if (new_entry != NULL) {
        new_entry->type = recv_entry->type;
        linkaddr_copy(&new_entry->dest,     &recv_entry->dest);
        linkaddr_copy(&new_entry->next_hop, &recv_entry->next_hop);
        new_entry->tot_hop = recv_entry->tot_hop;
        new_entry->metric  = recv_entry->metric;
        new_entry->seq_no  = recv_entry->seq_no;
        list_add(local_rt_table, new_entry);
      } 
      else {
        LOG_WARN("RT table full: could not add entry %d\n", i);
      }
    }
    LOG_INFO("Updated local routing table:\n");
    print_local_routing_table();
    print_adjacency_matrix();
    leds_single_off(LEDS_LED2);
  }
  else
  {
    LOG_INFO("Not the Master Node forwarding RT_REPORT_PACKET:\n");

    
    const linkaddr_t *dest = get_next_hop_to(&addr_master);
    nullnet_buf = (uint8_t *)&pkt;
    //nullnet_len = sizeof(pkt);
    nullnet_len = offsetof(struct rt_report_packet, table) + pkt->no_entries * sizeof(rt_entry);

    NETSTACK_NETWORK.output(dest); 
  }


}

 
static void HELLO_Callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  leds_single_on(LEDS_LED2);
  uint8_t type = *((uint8_t *)data);
  LOG_INFO("<< Received packet, type = %d, len = %d\n", type, len);
  

  switch(type) {
    case HELLO_PACKET:
      HELLO_PACKET_callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    case RT_REPORT_PACKET:
      RT_REPORT_PACKET_callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    default:
      LOG_WARN("Unknown packet type: %d\r\n", type);
     
  }
}

PROCESS(hello_process, "HELLO Flooding Process");
AUTOSTART_PROCESSES(&hello_process);
PROCESS_THREAD(hello_process, ev, data) {
  PROCESS_BEGIN();
  NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,GROUP_CHANNEL);
  radio_value_t channel;
  NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL, &channel);
  LOG_INFO("Radio channel set to %u\r\n", channel);

  //initiation the adjacency_matrix
  for (int i = 0; i < MAX_NODES; i++)
  for (int j = 0; j < MAX_NODES; j++)
    adjacency_matrix[i][j] = -1;  


  memb_init(&rt_mem);
  list_init(local_rt_table);
  nullnet_set_input_callback(HELLO_Callback);
  if(node_id == MASTER_NODE_ID) 
  {
    static struct hello_packet my_hello_pkt;
    static struct etimer timer;
    // periodically send hello packet
    etimer_set(&timer, CLOCK_SECOND * HELLO_INTERVAL);
    while(1) {
      last_seq_id = HELLO_SEQ_ID;
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
      leds_single_on(LEDS_LED1);
      my_hello_pkt.type = HELLO_PACKET;
      linkaddr_copy(&my_hello_pkt.src_master, &linkaddr_node_addr);
      my_hello_pkt.hop_count = 0;
      my_hello_pkt.seq_id = HELLO_SEQ_ID;
      forward_hello(&my_hello_pkt);
      LOG_INFO("MASTER broadcasted HELLO\r\n");
      leds_single_off(LEDS_LED1);
      etimer_reset(&timer);
    }
  }
  else{
    
  }
  PROCESS_END();
}


