/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2025 Chair of Communication Networks, TUM

   contributors:
   * Thomas Szyrkowiec
   * Mikhail Vilgelm
   * Octavio Rodríguez Cervantes
   * Angel Corona
   * Donjeta Elshani
   * Onur Ayan
   * Benedikt Hess
   * Philipp Langlechner

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   LESSON 5: Routing
*/

/*----------------------------INCLUDES----------------------------------------*/
// standard C includes:
#include <stdio.h>
#include <stdint.h>

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"    // Establish connections.
#include "net/netstack.h"      		// Wireless-stack definitions
#include "net/packetbuf.h"
#include "dev/button-hal.h" 		// User Button
#include "dev/leds.h"          		// Use LEDs.

// Header
#include "routing.h"
#include "buffer.h"
#include "helpers.h"

/*---------------------------Variables----------------------------------------*/
static uint8_t node_id;		        // Stores node id.

static linkaddr_t my_addr; 			// This node's link layer address. Value upon declaration: 
									// {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}

static uint8_t led_id;	        	// Each LED has a unique ID. When this
                                    // node generates a packet, it has this LED
                                    // ID.

static Buffer buffer;               // the buffer to store packets until they
                                    // are transmitted.


static l_table lut[TOTAL_NODES] = {

	/*
	 * First node lookup table
	 * */
	{
		// First entry
		.dest[0].u8[1] = 0x01, .next_hop[0].u8[1] = 0x01, .cost[0] = 0,
		// Second entry
		.dest[1].u8[1] = 0x02, .next_hop[1].u8[1] = 0x02, .cost[1] = 1,
		// Third entry
		.dest[2].u8[1] = 0x03, .next_hop[2].u8[1] = 0x02, .cost[2] = 2,
		/*
		 * Add here more entries if using more than three nodes.
		 */
	},

	/*
	 * Second node lookup table
	 * */
	{
		.dest[0].u8[1] = 0x01, .next_hop[0].u8[1] = 0x03, .cost[0] = 2,
		.dest[1].u8[1] = 0x02, .next_hop[1].u8[1] = 0x02, .cost[1] = 0,
		.dest[2].u8[1] = 0x03, .next_hop[2].u8[1] = 0x03, .cost[2] = 1,
	},

	/*
	 * Third node lookup table
	 * */
	{
		.dest[0].u8[1] = 0x01, .next_hop[0].u8[1] = 0x01, .cost[0] = 1,
		.dest[1].u8[1] = 0x02, .next_hop[1].u8[1] = 0x01, .cost[1] = 2,
		.dest[2].u8[1] = 0x03, .next_hop[2].u8[1] = 0x03, .cost[2] = 0,
	}
	,
	/*
	 * Add here more lookup table entries if using more than three nodes.
	 */
};

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(routing_process, "Lesson 3: Routing");
PROCESS(send_process, "Process to send packets");
AUTOSTART_PROCESSES(&routing_process, &send_process);

//------------------------ FUNCTIONS ------------------------

static void send_packet(packet_t tx_packet){
	uint8_t i;
	// Define next hop and forward packet
	for(i = 0; i < TOTAL_NODES; i++)
	{
		if(linkaddr_cmp(&tx_packet.dest, &lut[node_id - 1].dest[i]))
		{
			nullnet_buf = (uint8_t*) &tx_packet;
			nullnet_len = sizeof(packet_t);
			NETSTACK_NETWORK.output(&lut[node_id - 1].next_hop[i]);
			break;
		}
	}
	leds_single_off(tx_packet.message);
}


// sets a timer and puts this timer and the packet in the buffer
static void enqueue_packet(packet_t tx_packet){
	uint8_t return_code;
	struct timer packet_timer;

	timer_set(&packet_timer, CLOCK_SECOND * TIMER_INTERVAL);

	// put packet and timer in the queue
	return_code = BufferIn(&buffer, tx_packet, packet_timer);

	// check if storing was successful
	if (return_code == BUFFER_FAIL) {
		printf("The buffer is full. Consider increasing the 'BUFFER_SIZE' value"
				" in 'buffer.h'\n");
	} else {
		// inform the send process a new packet was enqueued
		process_post(&send_process, PROCESS_EVENT_MSG, 0);
	}
}


// Defines the behavior of a connection upon receiving data.                   //Change code here in Exercise 2 to make a endless loop
static void unicast_recv(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest){
	packet_t rx_packet;
	packet_t tx_packet;
	// Initialize all bits in tx_packet.dest to zero.
	linkaddr_copy(&tx_packet.dest, &linkaddr_null);

	packetbuf_copyto(&rx_packet);

	// For Debug purposes
	// printf("Unicast message received from 0x%x%x: '%s' [RSSI %d]\n",
	//		 from->u8[0], from->u8[1],
	//		(char *)packetbuf_dataptr(),
	//		(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));

	// Check if packet reached destination
	if(linkaddr_cmp(&rx_packet.dest, &linkaddr_node_addr))
	{
		//printf("Packet reached destination \n");
		printf("Packet reached destination at node %d\n", node_id);    //with code below transmission goes on

		// YOUR CODE HERE for exercise 2
		int next_id;    //return to next node id

		if(){
			
		}
		next_id =calculate_destination(node_id, TOTAL_NODES);

		tx_packet.dest.u8[0] = (next_id >> 8) & 0xFF;
		tx_packet.dest.u8[1] = next_id & 0xFF;
		tx_packet.message = rx_packet.message;

	}
	else
	{
		//no need to change here, if addr does not match simply forward the message
		tx_packet.dest.u8[0] = rx_packet.dest.u8[0];
		tx_packet.dest.u8[1] = rx_packet.dest.u8[1];
		tx_packet.message = rx_packet.message;
	}

	leds_single_on(rx_packet.message);
	enqueue_packet(tx_packet);
}


//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD(routing_process, ev, data) {

	PROCESS_BEGIN();

	static packet_t tx_packet;	// If it is not static, bits of its members might change unexpectedly.

	// Configure your team's channel (11 - 26).
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,12);           //Change the channel here, this time not in .h file anymore

	// Set this node's address.
	my_addr.u8[1] = 0x03;	// Change this value, so that this node will have the desired address and id.
	linkaddr_set_node_addr(&my_addr);

	print_settings();
	check_for_invalid_addr();
	node_id = (linkaddr_node_addr.u8[1] & 0xFF);
	led_id = get_led_id(node_id);
	print_lookup_table(lut[node_id - 1], TOTAL_NODES);

	// Initialize NullNet
	nullnet_set_input_callback(unicast_recv);

	while(1) {
		PROCESS_WAIT_EVENT();

		// check if button was pressed
		if(ev == button_hal_press_event)
		{
			// generate packet
			int dest_id = calculate_destination(node_id, TOTAL_NODES);
			tx_packet.dest.u8[0] = (dest_id >> 8) & 0xFF;
			tx_packet.dest.u8[1] = dest_id & 0xFF;
			tx_packet.message = led_id;

			leds_single_on(led_id);

			enqueue_packet(tx_packet);
		}
	}

	PROCESS_END();
}


PROCESS_THREAD(send_process, ev, data) {
	PROCESS_BEGIN();
	printf("send_process: started\n");

	static struct etimer t;
	static struct timer packet_timer;
	static packet_t tx_packet;
	static uint8_t return_code;
	static uint8_t is_processing_packet = 0;
	static clock_time_t remaining_time;

	while(1) {
		PROCESS_WAIT_EVENT();

		// a new packet has been added to the buffer
		if (ev == PROCESS_EVENT_MSG) {
			// is a packet already processed?
			if ( ! is_processing_packet ) {
				is_processing_packet = 1;
				// get a packet from the buffer
				return_code = BufferOut(&buffer, &tx_packet, &packet_timer);
				// there was nothing in the buffer
				if (return_code == BUFFER_FAIL){
					is_processing_packet = 0;
				}
				// there was something in the buffer
				else {
					remaining_time = timer_remaining(&packet_timer);
					// check if the timer has already expired
					if ( timer_expired(&packet_timer) ) {
						send_packet(tx_packet);
						is_processing_packet = 0;
						process_post(&send_process, PROCESS_EVENT_MSG, 0);
					}
					else {
						// wait for the remaining time
						etimer_set(&t, remaining_time);
					}
				}
			}
		} else if (ev == PROCESS_EVENT_TIMER) {
			// timer indicating time to send expired
			if (etimer_expired(&t)) {
				send_packet(tx_packet);
				is_processing_packet = 0;
				// tell the process to check if there is another packet in the
				// buffer
				process_post(&send_process, PROCESS_EVENT_MSG, 0);
			}
		}
	}
	PROCESS_END();
}
