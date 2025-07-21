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
   * Alexander Wietfeld

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   LESSON 2: MAC Configuration
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"	// Establish connections.
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "lib/random.h"
#include "dev/leds.h"
#include "dev/button-hal.h"

#include "rtimer.h"
#include "project-conf.h"

// Standard C includes:
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Support function includes:
#include "helpers.h"


typedef struct{
	uint16_t unique_id;					// Message ID
	char message[MAX_MESSAGE_LENGTH];			// Packet Message
}packet_t;

/* Global variables */
rtimer_clock_t tx_time[MAX_NUM_TX_PACKETS], rx_time;		// Stores packet transmission time.
float rtt[MAX_NUM_TX_PACKETS] = {0};				// Stores packet round-trip time
uint8_t packet_counter = 0;					// Counts transmitted packet.

/* Packet to be transmitted.*/
packet_t tx_packet[MAX_NUM_TX_PACKETS];

/* Runtime configuration variables */
static uint16_t current_inter_packet_time;
static uint8_t current_total_tx_packets;


/*** CONNECTION DEFINITION***/

/**
* @param message - message to be broadcasted
*/
void echo_packet(packet_t * packet) {
	nullnet_buf = (uint8_t *)packet;
	nullnet_len = sizeof(packet_t);
	NETSTACK_NETWORK.output(NULL);

}

/**
 * Callback function for received packet processing.
 *
 */
void broadcast_recv(const void *data, uint16_t len,
  					const linkaddr_t *src, const linkaddr_t *dest) {

	packet_t rx_packet;
	packetbuf_copyto(&rx_packet);
	rx_time = RTIMER_NOW();
	/* Get packet's RSSI */
	signed short last_rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

	uint8_t id = rx_packet.unique_id;

	leds_single_on(LEDS_LED2);
	printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",src->u8[0], src->u8[1],len,last_rssi);
			
	/* Check type of packet. */
	if (strcmp(rx_packet.message, "Request") == 0) {
		strcpy(rx_packet.message, "Reply");
		echo_packet(&rx_packet);	// Echo received packet.
		printf("Request received - Packet id. %d\n", id);
	}
	else {
		rtt[id] = (rx_time - tx_time[id]);	// Calculate round trip time, in ticks.
		rtt[id] = (float) rtt[id]/RTIMER_SECOND;						// Convert ticks to seconds.
		char rtt_buffer[5];									// Buffer to store Round-trip time.
		ftoa(rtt[id], rtt_buffer, 3);								// Covert Round-trip time from float to char.
		printf("Reply received - Packet id. %d RRT= %s seconds. \n", id, rtt_buffer);
	}

	leds_single_off(LEDS_LED2);
}

/*** CONNECTION DEFINITION END ***/

/*** MAIN PROCESS DEFINITION ***/
PROCESS(mac_process, "Lesson 2: MAC settings");
AUTOSTART_PROCESSES(&mac_process);

/*** MAIN THREAD ***/
PROCESS_THREAD(mac_process, ev, data) {

	static struct etimer et;
	static int i;
	static int packets_lost;
	float mean_time, packet_delivery_ratio;
	char mean_time_buffer[5], packet_delivery_ratio_buffer[6];	// Buffer to store chars from float

	PROCESS_EXITHANDLER(printf("mac_process terminated!\n"));
	PROCESS_BEGIN();

	/*
	 * set your group's channel
	 */
	NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, CHANNEL);

	/*
	 * Change the transmission power
	 */
	NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER);

	// Initialize NullNet
	nullnet_set_input_callback(broadcast_recv);

	while(1){

		/* Wait until transmission activated by pushing user button. */
		PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_press_event);

		/* Obtain button event data using button_hal_button_t. */
		button_hal_button_t *btn = (button_hal_button_t *) data;
		
		/* Select configuration based on which button was pressed. */
		if(btn==button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ZERO)){
			current_inter_packet_time = INTER_PACKET_TIME_BUTTON_1;
			current_total_tx_packets  = TOTAL_TX_PACKETS_BUTTON_1;
			printf("Configuration: Button 1 selected: INTER_PACKET_TIME = %u ticks, TOTAL_TX_PACKETS = %u\n",
				current_inter_packet_time, current_total_tx_packets);
		}
		else if(btn==button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ONE)){
			current_inter_packet_time = INTER_PACKET_TIME_BUTTON_2;
			current_total_tx_packets  = TOTAL_TX_PACKETS_BUTTON_2;
			printf("Configuration: Button 2 selected: INTER_PACKET_TIME = %u ticks, TOTAL_TX_PACKETS = %u\n",
				current_inter_packet_time, current_total_tx_packets);
		}
		else if(btn==button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_TWO)){
			current_inter_packet_time = INTER_PACKET_TIME_BUTTON_3;
			current_total_tx_packets  = TOTAL_TX_PACKETS_BUTTON_3;
			printf("Configuration: Button 3 selected: INTER_PACKET_TIME = %u ticks, TOTAL_TX_PACKETS = %u\n",
				current_inter_packet_time, current_total_tx_packets);
		}
		else if(btn==button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_THREE)){
			current_inter_packet_time = INTER_PACKET_TIME_BUTTON_4;
			current_total_tx_packets  = TOTAL_TX_PACKETS_BUTTON_4;
			printf("Configuration: Button 4 selected: INTER_PACKET_TIME = %u ticks, TOTAL_TX_PACKETS = %u\n",
				current_inter_packet_time, current_total_tx_packets);
		}

		/* Reset variables. */
		packet_counter = 0;

		/* Generate and transmit packets.*/
		while(packet_counter < current_total_tx_packets)
		{
			/*
			 * Wait Inter-Packet Arrival Time.
			 * */
			etimer_set(&et, current_inter_packet_time);

			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

			leds_single_on(LEDS_LED1);

			/* Generate packet. */
			tx_packet[packet_counter].unique_id = packet_counter;
			strcpy(tx_packet[packet_counter].message, "Request");

			/*
			 * fill the packet buffer & send the packet
			 */
			nullnet_buf = (uint8_t *)&tx_packet[packet_counter];
			nullnet_len = sizeof(packet_t);
			NETSTACK_NETWORK.output(NULL);

			/* Store packet sending time. */
			tx_time[packet_counter] = RTIMER_NOW();

			/*
			 * reset the timer
			 */
			etimer_reset(&et);

			leds_single_off(LEDS_LED1);
			packet_counter++;
		}

		etimer_set(&et, 5*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		/* Plot configuration data again. */
		printf("\n--- Configuration Summary ---\n");
		printf("INTER_PACKET_TIME = %u ticks\n", current_inter_packet_time);
		printf("TOTAL_TX_PACKETS = %u\n", current_total_tx_packets);

		/* Calculate number of lost packets, by checking Round trip time.
		 * If a packet was not received its rtt = 0;
		 */
		packets_lost = 0;
		for(i = 0; i<current_total_tx_packets; i++)
		if(rtt[i] == 0)
			packets_lost++;
		packet_delivery_ratio = (float)(current_total_tx_packets-packets_lost)/(current_total_tx_packets)*100;
		ftoa(packet_delivery_ratio, packet_delivery_ratio_buffer,2);		
		printf("\nTotal number of packets lost = %d.\n", packets_lost);
		printf("Packet delivery rate = %s %%.\n", packet_delivery_ratio_buffer);

		/*
		 * Calculate packet transmission mean time.
		 * Exclude lost packets.
		 * */
		mean_time = 0;
		for(i = 0; i<current_total_tx_packets; i++)
			if(rtt[i] != 0)
				mean_time = mean_time + rtt[i];
		mean_time = (float)(mean_time/(current_total_tx_packets-packets_lost)); // Divide by total round-trip time by total packets send.
		ftoa(mean_time, mean_time_buffer,3);
		printf("Mean round-trip time = %s seconds. \n", mean_time_buffer);


	}

	PROCESS_END();
}
