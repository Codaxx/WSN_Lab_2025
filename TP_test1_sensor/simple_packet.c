#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "dev/leds.h"
#include "net/linkaddr.h"
#include "sys/node-id.h"
#include "sys/log.h"
#include "net/linkaddr.h"
#include <string.h>
#include <stdio.h>

#include "packet_structure.h"
#include "project-conf.h"

/**
 * @brief Convert a link-layer address (linkaddr_t) into a 16-bit node ID.
 *
 * @param addr Pointer to a linkaddr_t structure (e.g., from packet source)
 * @return uint16_t The corresponding node ID
 */
uint16_t get_node_id_from_linkaddr(const linkaddr_t *addr) {
  return ((uint16_t)addr->u8[LINKADDR_SIZE - 2] << 8) | addr->u8[LINKADDR_SIZE - 1];
}

// used for recording the packet seq, prevent loop
static uint16_t last_seq_id = 0;


// Forward packet via broadcast
void forward_hello(struct hello_packet *pkt)
{
  nullnet_buf = (uint8_t *)pkt;
  nullnet_len = sizeof(*pkt);
  NETSTACK_NETWORK.output(NULL);
}

// Reply to hello packet
static void reply_to_hello(const linkaddr_t *dest, uint8_t hop, int8_t rssi)
{
  struct node_reply_packet pkt;
  pkt.type = RE_HELLO_PACKET;
  linkaddr_copy(&pkt.node_addr, &linkaddr_node_addr);
  pkt.hop_count = hop;
  uint16_t dst_node_id = get_node_id_from_linkaddr(dest);
  LOG_INFO("REPLY to id %i | my id:%i | hop=%d | rssi=%d\n",
       dst_node_id,
       node_id,
       hop, rssi);
  nullnet_buf = (uint8_t *)&pkt;
  nullnet_len = sizeof(pkt);
  NETSTACK_NETWORK.output(dest); 
}

// Receive hello packet callback
// 1.forward hello packet
// 2.reply to the src node
static void HELLO_PACKET_callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  leds_single_on(LEDS_LED2);

  if(len != sizeof(struct hello_packet)) {
    LOG_WARN("Invalid packet size %u\n", len);
    leds_single_off(LEDS_LED2);
    return;
  }
  // processing the hello packet info
  struct hello_packet *pkt = (struct hello_packet *)data;
  int8_t rssi = (int8_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  uint16_t src_node_id = get_node_id_from_linkaddr(src);
  
  // Avoid loops: if already seen, drop
  if(pkt->seq_id <= last_seq_id) {
    leds_single_off(LEDS_LED2);
    return;
  }
  LOG_INFO("HELLO from node id %i | seq %u | hop %u | rssi%i",
         src_node_id,
         pkt->seq_id, pkt->hop_count, rssi);
  // Update sequence tracking
  last_seq_id = pkt->seq_id;
  // Increment hop count
  pkt->hop_count++;
  // to do inint node_info and ip_table
  // initialization ip_table, and add flooding info

  // Forward the packet
  forward_hello(pkt);
  // Reply the source
  reply_to_hello(&pkt->originator, pkt->hop_count, rssi);

  leds_single_off(LEDS_LED2);
}

static void RE_HELLO_PACKET_Callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  struct node_reply_packet *pkt = packetbuf_dataptr();
  if(pkt->type == RE_HELLO_PACKET) {
    LOG_INFO("REPLY from id %i | hop=%d" ,
           node_id,
           pkt->hop_count);
  }
}
static void HELLO_Callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  leds_single_on(LEDS_LED2);
  uint8_t type = *((uint8_t *)data);
  if(len < 1) 
  {
    LOG_WARN("Unknown packet type: %d\n", type); 
    leds_single_off(LEDS_LED2);
    return;
  }
  

  switch(type) {
    case HELLO_PACKET:
      HELLO_PACKET_callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    case RE_HELLO_PACKET:
      RE_HELLO_PACKET_Callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    default:
      LOG_WARN("Unknown packet type: %d\n", type);
      //printf("Unknown packet type: %d\n", type);
  }
}

PROCESS(hello_process, "HELLO Flooding Process");
AUTOSTART_PROCESSES(&hello_process);
PROCESS_THREAD(hello_process, ev, data) {
  PROCESS_BEGIN();
  NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,GROUP_CHANNEL);
  nullnet_set_input_callback(HELLO_Callback);
  if(node_id == MASTER_NODE_ID) {

    static struct hello_packet my_hello_pkt;
    last_seq_id = 1;
    static struct etimer timer;
    // periodically send hello packet
    etimer_set(&timer, CLOCK_SECOND * HELLO_INTERVAL);
    while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
      leds_single_on(LEDS_LED1);
      my_hello_pkt.type = HELLO_PACKET;
      linkaddr_copy(&my_hello_pkt.originator, &linkaddr_node_addr);
      my_hello_pkt.hop_count = 0;
      my_hello_pkt.seq_id = HELLO_SEQ_ID;
      forward_hello(&my_hello_pkt);
      LOG_INFO("MASTER broadcasted HELLO\n");
      leds_single_off(LEDS_LED1);
      etimer_reset(&timer);
    }
  }
  PROCESS_END();
}


