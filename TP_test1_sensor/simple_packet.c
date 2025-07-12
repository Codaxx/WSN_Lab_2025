#define LOG_LEVEL LOG_LEVEL_INFO
#define LOG_MODULE "LEACH"


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
#include <stdlib.h>

#include "common/saadc-sensor.h"		// Saadc to read Battery Sensor.
#include "common/temperature-sensor.h"
#include "my_sensor.h"
#include "my_functions.h"
#include "packet_structure.h"
#include "project-conf.h"

// used for recording the packet seq, prevent loop
static uint16_t last_seq_id = 0;
static linkaddr_t addr_master;
static short adjacency_matrix[MAX_NODES][MAX_NODES];  
static linkaddr_t node_index_to_addr[MAX_NODES]; 
static int num_known_nodes = 0;

// sensor data transmission
static uint8_t trans_flag;
static int distance_buffer[BUFFER_SIZE], light_buffer[BUFFER_SIZE], temperature_buffer[BUFFER_SIZE];
static int global_index;
static int distance_av_0, light_av_0, temperature_av_0;
static int distance_av_1, light_av_1, temperature_av_1;
static sensor_data recv_message;
const float battery[MASTER_NODE_ID] = {0.8f,0.9f, 0.7f, 1.0f, 0.9f, 0.8f, 0.6f, 0.8f,0.3f,0.6f};

LIST(local_rt_table);
MEMB(rt_mem,rt_entry,MAX_NODES);

LIST(permanent_rt_table);
MEMB(permanent_rt_mem,rt_entry,MAX_NODES);




//sensor data 
void distance_av_cal(){
	distance_av_1 = distance_av_0;
	for(int i=0; i<BUFFER_SIZE; i++){
		distance_av_0 += distance_buffer[i];
	}
	distance_av_0 /= BUFFER_SIZE;
}

void light_av_cal(){
	light_av_1 = light_av_0;
	for(int i=0; i<BUFFER_SIZE; i++){
		light_av_0 += light_buffer[i];
	}
	light_av_0 /= BUFFER_SIZE;
}

void temperture_av_cal(){
	temperature_av_1 = temperature_av_0;
	for(int i=0; i<BUFFER_SIZE; i++){
		temperature_av_0 += temperature_buffer[i];
	}
	temperature_av_0 /= BUFFER_SIZE;
}

// routing discovery part 
rt_entry * check_local_rt(const linkaddr_t *addr)
{ 
  rt_entry *e= list_head(local_rt_table);
	for(; e != NULL; e = e->next)
	{
		if(linkaddr_cmp(&e->dest, addr))
		{
			return e;
		}
	}
	return e;
}

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
  return NULL;  
}

void print_local_routing_table() {
  LOG_INFO("+------------------+ Local Routing Table: +--------------------+\n");
  int i = 0;
  for(rt_entry *e = list_head(local_rt_table); e != NULL; e = e->next) {
    uint16_t dest_id = get_node_id_from_linkaddr(&e->dest);
    uint16_t next_id = get_node_id_from_linkaddr(&e->next_hop);
    LOG_INFO("|No.%d | dest:%u | next:%u | tot_hop:%u | rssi:%d | seq:%u |\n",
            i++, dest_id, next_id,
            e->tot_hop, e->metric, e->seq_no);
  }
  LOG_INFO("+------------------+ ----------------------+--------------------+\n");
}

void forward_hello(struct dio_packet *pkt)
{
  nullnet_buf = (uint8_t *)pkt;
  nullnet_len = sizeof(*pkt);
  NETSTACK_NETWORK.output(NULL);
}

void insert_entry_to_rt_table(const linkaddr_t *dst, const linkaddr_t *next_hop,uint8_t tot_hop, int16_t metric,uint16_t seq_no)
{
  rt_entry *e = memb_alloc(&rt_mem);
  if(e != NULL) {
    linkaddr_copy(&e->dest, dst);
    linkaddr_copy(&e->next_hop, next_hop);
    e->tot_hop = tot_hop;
    e->metric = metric;
    e->seq_no = seq_no;
    list_add(local_rt_table, e);  
  }
}

void patch_update_local_rt_table(const linkaddr_t *dst, const linkaddr_t *next,uint8_t tot_hop, int16_t metric,uint16_t seq_no)
{
  rt_entry* e = check_local_rt(dst);
    if (e == NULL)
    {
      insert_entry_to_rt_table(dst,dst,tot_hop,metric,seq_no);
      LOG_INFO("Discover new nodes adding to the rt table:\n");
    }
    else 
    {
      if(linkaddr_cmp(&e->dest, &linkaddr_node_addr))
      { 
        return;
      }
      else if(e->seq_no < seq_no)
      {
        if (abs(e->metric - metric) > 5)   
        {
          LOG_INFO("Update rt table with new metric:\n");
          e->metric = metric;
        }
      } 

    }



}

void print_rt_entries_pkt(const struct rt_entry_pkt *pkt)
{
  printf("DAO Packet from node ");
  uint16_t pkt_src_id = get_node_id_from_linkaddr(&pkt->src);
  printf("%u", pkt_src_id);
  printf("+-----------------------------------------------------------------------------------+\n");
  printf("|src   |  dest  | next_hop | hops | metric | seq_no |\n");
  printf("+-----------------------------------------------------------------------------------+\n");
    uint16_t src_id  = get_node_id_from_linkaddr(&pkt->rt_src);
    uint16_t dest_id = get_node_id_from_linkaddr(&pkt->rt_dest);
    uint16_t next_id = get_node_id_from_linkaddr(&pkt->rt_next_hop);
    printf("| %5u  |  %5u |   %5u  |  %3u |  %5d |  %5u |\n",
           src_id,
           dest_id,
           next_id,
           pkt->rt_tot_hop,
           pkt->rt_metric,
           pkt->rt_seq_no);

  printf("+-----------------------------------------------------------------------------------+\n");
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

static void routing_report(const linkaddr_t *dest, uint8_t hop, int8_t rssi, uint16_t seq_id)
{
  static struct rt_entry_pkt pkt;
  //memset(&pkt, 0, sizeof(pkt));
  pkt.type = RT_REPORT_PACKET;
  linkaddr_copy(&pkt.src, &linkaddr_node_addr);
  pkt.hop_count = 0;
  pkt.seq_id = seq_id;
  rt_entry *iter = list_head(local_rt_table);
  linkaddr_copy(&pkt.rt_src,     &iter->dest);
  for(; iter != NULL; iter = iter->next) {
    
    uint16_t dest_id = get_node_id_from_linkaddr(&iter->dest);
    uint16_t next_id = get_node_id_from_linkaddr(&iter->next_hop);
    printf("  dest: %i\n",dest_id );
    printf("  next_hop: %i\n", next_id);
    printf("  tot_hop: %d\n", iter->tot_hop);
    printf("  metric: %d\n", iter->metric);
    printf("  seq_no: %u\n", iter->seq_no);
    
    linkaddr_copy(&pkt.rt_dest,     &iter->dest);
    linkaddr_copy(&pkt.rt_next_hop, &iter->next_hop);
    pkt.rt_tot_hop = iter->tot_hop;
    pkt.rt_metric  = iter->metric;
    pkt.rt_seq_no  = iter->seq_no;


    /*printf("Entry %d: dest=%i next_hop=%i hop=%d metric=%d\n",
      dest_id,
      next_id,
      pkt.tot_hop,
      pkt.metric);*/
    clock_wait(CLOCK_SECOND / 20);  // 等待 50ms
    nullnet_buf = (uint8_t *)&pkt;
    nullnet_len = sizeof(pkt);
    NETSTACK_NETWORK.output(dest); 

  }
  LOG_INFO("I have sent RT_REPORT_PACKET\n");
  uint16_t dest_id = get_node_id_from_linkaddr(dest);
  printf("Sending packet to dest: %i\n", dest_id);
}

// ch choosing part
void advertise_node_addr(uint8_t* link_table)
{
  for (int i=1;i<num_known_nodes;i++) {
    int find_flag = 0;
    static linkaddr_t advertise_ch_addr;
    for (int dst = 0; dst < num_known_nodes; dst++) {
      // LOG_INFO("-----------------------------\n\r");
      // LOG_INFO("%d \n\r", link_table[i * MAX_NODES + dst]);
      if (link_table[i*MAX_NODES + dst]== 1) {     
        advertise_ch_addr = node_index_to_addr[dst];
        find_flag = 1;
        break;
      }
    }
    if(!find_flag){
      LOG_WARN("Can't find the routing\n\r");
      return;
    }
    else{
      LOG_INFO("FIND CONECTION\n\r");
    }

    struct advertise_packet pkt;
    pkt.type = ADVERTISE_PACKET;
    pkt.dest = node_index_to_addr[i]; 
    pkt.advertise_ch = advertise_ch_addr; 
    pkt.seq_id = 5;
    LOG_INFO("Sending ADVERTISE packet:\n");
    LOG_INFO("  Dest node:       %u\n", get_node_id_from_linkaddr(&node_index_to_addr[i]));
    LOG_INFO("  CH node:    %u\n", get_node_id_from_linkaddr(&advertise_ch_addr));
    LOG_INFO("  Seq ID:          %u\n", pkt.seq_id);
    nullnet_buf = (uint8_t *)&pkt;
    nullnet_len = sizeof(pkt);
    NETSTACK_NETWORK.output(&node_index_to_addr[i]);   
  }

}

// Receive hello packet callback
// 1.forward hello packet
// 2.reply to the src node
// 3,adding the hello packet info from the rt_table
static void DIO_PACKET_callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  int8_t rssi = (int8_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  //LOG_INFO("Hello Packet Process begin\r\n");
  if(rssi <= RSSSI_TH)
  {
    LOG_WARN("low RSSI DAO, rejected\n\r");
    return;
  }
  // processing the hello packet info
  if(node_id == MASTER_NODE_ID) 
  {
    leds_single_off(LEDS_LED2);
    return;
  }

  leds_single_on(LEDS_LED2);
  struct dio_packet *pkt = (struct dio_packet *)data;
  linkaddr_t report_src;
  linkaddr_copy(&addr_master, &pkt->src_master);
  linkaddr_copy(&report_src, &pkt->src);
  
  // Avoid loops: if already seen, drop
  if((pkt->seq_id <last_seq_id) && parent_is_in_rt_table(&report_src)){
    leds_single_off(LEDS_LED2);
    LOG_INFO("The Packet has been Processed\r\n");
    return;
  }
  last_seq_id = pkt->seq_id;
  // processing the packet info
  
  //pkt->hop_count++;
  linkaddr_copy(&pkt->src, &linkaddr_node_addr);

  // initialization ip_table, and add flooding info
  // update_local_rt_table(master_node info + hello packet info);
  // flooding connectivity

  // adding the hello pkt_src to the routing table
  patch_update_local_rt_table(&report_src,&report_src,1,rssi,pkt->seq_id);

  // other nodes adding the master node hop to the routing table
  if (!linkaddr_cmp(&report_src, &pkt->src_master))
  {
    patch_update_local_rt_table(&pkt->src_master,&report_src,pkt->hop_count,rssi,pkt->seq_id);
  }

  // print the local_rt_table
  LOG_INFO("Local routing tabel is listed as follw: \r\n");
  print_local_routing_table();
  // Forward the packet
  forward_hello(pkt);
  // Reply the true source
  routing_report(&report_src, pkt->hop_count, rssi,pkt->seq_id);

  leds_single_off(LEDS_LED2);
}

static void DAO_PACKET_callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  leds_single_on(LEDS_LED2);
  LOG_INFO("Receiving RT_REPORT_PACEKT:\n");
  struct rt_entry_pkt *pkt = (struct rt_entry_pkt *)data;
  pkt->hop_count++;
  int8_t rssi = (int8_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  // avoid unstalbe link hard decision
  if(rssi <= RSSSI_TH)
  {
    LOG_WARN("low RSSI DAO, rejected\n\r");
    return;
  }
  patch_update_local_rt_table(src,src,pkt->hop_count,rssi,pkt->seq_id);

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
    int dst_index = get_index_from_addr(&pkt->rt_dest);
    if (src_index == dst_index) 
    {
      adjacency_matrix[src_index][dst_index] = 255;
    }
    else
    {
      adjacency_matrix[src_index][dst_index] = pkt->rt_metric;
      adjacency_matrix[dst_index][src_index] = pkt->rt_metric;
    }
    // go through the routing report rt_table, update the local rt table
    // note that next hop would be the packet src 
    patch_update_local_rt_table(&pkt->rt_dest,src,pkt->rt_tot_hop+1,pkt->rt_metric,pkt->rt_seq_no);
    leds_single_off(LEDS_LED2);
  }
  else
  {
    // update my own local routing table
    // since local routing table update, routing report

    //routing_report(&addr_master, pkt->hop_count, rssi,pkt->seq_id);
    LOG_INFO("Not the Master Node forwarding RT_REPORT_PACKET:\n");
    const linkaddr_t *dest = get_next_hop_to(&addr_master);
    nullnet_buf = (uint8_t *)&pkt;
    nullnet_len = sizeof(pkt);
    NETSTACK_NETWORK.output(dest); 
  }


}

static void SENSOR_PACKET_callback(const void *data, uint16_t len, 
                            const linkaddr_t *src, const linkaddr_t *dest){
  int8_t rssi = (int8_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  //LOG_INFO("Hello Packet Process begin\r\n");
  if(rssi <= RSSSI_TH)
  {
    LOG_WARN("low RSSI DAO, rejected\n\r");
    return;
  }                    
	memcpy(&recv_message, (sensor_data*)data, sizeof(recv_message));
	if(recv_message.type == 3){
		LOG_INFO("Received data are from %d:\n\r", get_node_id_from_linkaddr(&recv_message.source));
		LOG_INFO("batttery: [%d](mV)\n\r", recv_message.battery);
		LOG_INFO("temperature: [%d](C)\n\r", recv_message.temperature);
		LOG_INFO("light: [%d](lux)\n\r", recv_message.light_lux);
		LOG_INFO("distance: [%d](cm)\n\r", recv_message.distance);
	}
}
 

static void ADVERTISE_PACKET_callback(const void *data, uint16_t len, 
                            const linkaddr_t *src, const linkaddr_t *dest) {
  if(len != sizeof(struct advertise_packet)) {
    LOG_WARN("Wrong packet size: %u\n", len);
    return;
  }

  struct advertise_packet *pkt = (struct advertise_packet *)data;
  int8_t rssi = (int8_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  LOG_INFO("Geting ADVERTISE packet:\n");
  LOG_INFO("  Dest node:       %u\n", get_node_id_from_linkaddr(&pkt->dest));
  LOG_INFO("  CH node:    %u\n", get_node_id_from_linkaddr(&pkt->advertise_ch));
  LOG_INFO("  Seq ID:          %u\n", pkt->seq_id);
  //LOG_INFO("Hello Packet Process begin\r\n");
  if(rssi <= -75)
  {
    LOG_WARN("low RSSI DAO, rejected\n\r");
    return;
  }
  if(linkaddr_cmp(&pkt->dest, &linkaddr_node_addr)) {
    // my dest 
    //linkaddr_cmp(&master_addr,&pkt->advertise_ch);
    memb_init(&permanent_rt_mem);
    list_init(permanent_rt_table);
    rt_entry *e = memb_alloc(&permanent_rt_mem);
    if(e != NULL) {
      linkaddr_copy(&e->dest, &pkt->dest);
      linkaddr_copy(&e->next_hop, &pkt->dest);
      e->tot_hop = 1;
      e->metric = rssi;
      e->seq_no = 1;
      list_add(permanent_rt_table, e);
      uint16_t dest_id = get_node_id_from_linkaddr(&e->dest);
      uint16_t next_id = get_node_id_from_linkaddr(&e->next_hop);
      LOG_INFO("+------------------+ Permanent Routing Table: +--------------------+\n");
      LOG_INFO("|No.0 | dest:%u | next:%u | tot_hop:%u | rssi:%d | seq:%u |\n",
             dest_id, next_id,
            e->tot_hop, e->metric, e->seq_no);
      LOG_INFO("+------------------+ ------------------------ +--------------------+\n");

    }

  } else {
    // not my dest
    nullnet_buf = (uint8_t *)pkt;
    nullnet_len = sizeof(struct advertise_packet);
    NETSTACK_NETWORK.output((get_next_hop_to(&pkt->dest)));

  }
}


static void HELLO_Callback(const void *data, uint16_t len,
                           const linkaddr_t *src, const linkaddr_t *dest)
{
  leds_single_on(LEDS_LED2);
  uint8_t type = *((uint8_t *)data);
  //LOG_INFO("<< Received packet, type = %d, len = %d\n", type, len);
  switch(type) {
    case HELLO_PACKET:
      DIO_PACKET_callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    case RT_REPORT_PACKET:
      DAO_PACKET_callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    case SENSOR_DATA_PACKET:
      SENSOR_PACKET_callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    case ADVERTISE_PACKET:
      ADVERTISE_PACKET_callback(data, len, src, dest);
      leds_single_off(LEDS_LED2);
      break;
    default:
      LOG_WARN("Unknown packet type: %d\r\n", type);
     
  }
}

PROCESS(hello_process, "HELLO Flooding Process");
PROCESS(sensro_report_process, "Hello Dummy Process");
PROCESS(delivery_ch_process, "choosing CH Process");


AUTOSTART_PROCESSES(&hello_process, &sensro_report_process,&delivery_ch_process);

PROCESS_THREAD(hello_process, ev, data) {
  static struct etimer timer;
  static struct dio_packet my_hello_pkt;
  static uint8_t hello_process_cnt = 0;

  PROCESS_BEGIN();

  LOG_INFO("HELLO PROCESS BEGIN\n");
  NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,GROUP_CHANNEL);
  radio_value_t channel;
  NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL, &channel);
  LOG_INFO("Radio channel set to %u\r\n", channel);
  if (NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER) != RADIO_RESULT_OK) {
    LOG_WARN("Failed to set TX power to %d dBm\n", TX_POWER);
  } else {
    LOG_INFO("TX power set to %d dBm\n", TX_POWER);
  }

  get_index_from_addr(&linkaddr_node_addr);
  for (int i = 0; i < MAX_NODES; i++) {
    for (int j = 0; j < MAX_NODES; j++) {
      adjacency_matrix[i][j] = (i == j) ? 255 : 0;
    }
  }
  memb_init(&rt_mem);
  list_init(local_rt_table);
  insert_entry_to_rt_table(&linkaddr_node_addr, &linkaddr_node_addr, 0, 0, 0);

  nullnet_set_input_callback(HELLO_Callback);
  last_seq_id = 1;
  
  if(node_id == MASTER_NODE_ID) {
    etimer_set(&timer, CLOCK_SECOND * HELLO_INTERVAL);
    while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

      if(hello_process_cnt >= 10) {
        LOG_INFO("HELLO process complete. Exiting.\n");
        PROCESS_EXIT();  
      }

      leds_single_on(LEDS_LED1);
      my_hello_pkt.type = HELLO_PACKET;
      linkaddr_copy(&my_hello_pkt.src, &linkaddr_node_addr);
      linkaddr_copy(&my_hello_pkt.src_master, &linkaddr_node_addr);
      my_hello_pkt.hop_count = 0;
      my_hello_pkt.seq_id = last_seq_id++;

      forward_hello(&my_hello_pkt);
      LOG_INFO("MASTER broadcasted HELLO %d\r\n", hello_process_cnt+1);
      leds_single_off(LEDS_LED1);

      hello_process_cnt++;
      etimer_reset(&timer);
    }
  }

  PROCESS_END();
}

PROCESS_THREAD(sensro_report_process, ev, data)
{
  static struct etimer sensor_reading_timer;
	static int light_raw, distance_raw;
	static int light_value, distance_value;
	static int voltage, temperature;
	static sensor_data packet;
  PROCESS_BEGIN();
  etimer_set(&sensor_reading_timer, CLOCK_SECOND*3);

  if(node_id == MASTER_NODE_ID) {
  }
  else{
    while(1) {
      
      PROCESS_WAIT_EVENT();
      // read raw data from ADC
      light_raw = saadc_sensor.value(P0_30);
      distance_raw = saadc_sensor.value(P0_31);
      
      // get sensor data
      light_value = get_light_lux(light_raw);
      distance_value = get_distance(distance_raw);
      voltage = get_millivolts(saadc_sensor.value(BATTERY_SENSOR));
      temperature = temperature_sensor.value(0)/4;

      distance_buffer[global_index] = distance_value;
      light_buffer[global_index] = light_value;
      temperature_buffer[global_index] = temperature;
      global_index ++;
      if(global_index>=BUFFER_SIZE){
        distance_av_cal();
        light_av_cal();
        temperture_av_cal();
        global_index = 0;
        trans_flag = 1;
      }
      if(ABS(distance_av_1-distance_av_0) >= DIS_THRES && ABS(light_av_1-light_av_0) >= LIGHT_THRES){
        trans_flag = 1;
      }
      // assign data to packet;
      if(trans_flag){
        packet.type = 3;
        linkaddr_copy(&packet.source, &linkaddr_node_addr);
        packet.light_lux = light_av_0;
        packet.distance = distance_av_0;
        packet.battery = voltage;
        packet.temperature = temperature;

        // transmit data to master
        const linkaddr_t *next_hop = get_next_hop_to(&addr_master);
        if(next_hop != NULL) {
          nullnet_buf = (uint8_t *)&packet;
          nullnet_len = sizeof(packet);
          NETSTACK_NETWORK.output(next_hop);
          LOG_INFO("Sent sensor data to master. Temp=%i, Distance=%i, Battery=%i, Light_Lux%i\n",
            packet.temperature,packet.distance,packet.battery,packet.light_lux );
        } else {
          LOG_WARN("No route to master!\n");
        }

        trans_flag = 0;
      }  
      etimer_reset(&sensor_reading_timer);
    }
   
  }
  

  PROCESS_END();
}

PROCESS_THREAD(delivery_ch_process, ev, data)
{
	static struct etimer choose_timer;
  PROCESS_BEGIN();
  etimer_set(&choose_timer, CLOCK_SECOND*3);
	while(1){
		PROCESS_WAIT_EVENT();
    unsigned char head_list[3] ={0};
    short* rssi = (short*)adjacency_matrix;
		volatile static unsigned char link_table[MAX_NODES*MAX_NODES]= {0};
    print_local_routing_table();
    print_adjacency_matrix();
    // todo the adjacency_matrix need to stable, rssi need to large -30
		from_rssi_to_link(rssi, battery, MAX_NODES, (uint8_t*)link_table,head_list,1);
    memb_init(&permanent_rt_mem);
    list_init(permanent_rt_table);
    if(node_id == MASTER_NODE_ID)
    {
      LOG_INFO("+------------------+ Permanent Routing Table: +--------------------+\n");
      for(int i=0;i<3;i++)
      {
        rt_entry *e = memb_alloc(&permanent_rt_mem);
        if(e != NULL) {
          
          linkaddr_copy(&e->dest, &node_index_to_addr[++head_list[i]]);
          linkaddr_copy(&e->next_hop, &node_index_to_addr[++head_list[i]]);
          e->tot_hop = 1;
          e->metric = rssi[++head_list[i]];
          e->seq_no = 1;
          list_add(permanent_rt_table, e);
          uint16_t dest_id = get_node_id_from_linkaddr(&e->dest);
          uint16_t next_id = get_node_id_from_linkaddr(&e->next_hop);
         
          LOG_INFO("|No.%i | dest:%u | next:%u | tot_hop:%u | rssi:%d | seq:%u |\n",
                i,dest_id, next_id,
                e->tot_hop, e->metric, e->seq_no);
          
      }
      LOG_INFO("+------------------+ ------------------------ +--------------------+\n");




      }
      



    }
    advertise_node_addr((uint8_t*)link_table);
		etimer_reset(&choose_timer);
	}

	PROCESS_END();
}