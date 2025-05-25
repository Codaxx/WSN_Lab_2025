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
   * Xianglong Wang
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

   LESSON 2: Simple packet
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"	// Establish connections.
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "lib/random.h"
#include "dev/leds.h"

// Standard C includes:
#include <string.h>
#include <stdio.h>
#include <stdint.h>


/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/
PROCESS(simple_packet_process, "Lesson 2: Simple packet");
AUTOSTART_PROCESSES(&simple_packet_process);

/*---------------------------------------------------------------------------*/
/* Callback function for received packet processing. 						 */
/*---------------------------------------------------------------------------*/
void broadcast_recv(const void *data, uint16_t len,
  					const linkaddr_t *src, const linkaddr_t *dest) {
	leds_single_on(LEDS_LED2);

	/* Get packet's RSSI */
	signed short last_rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

	/* Print address of sending node */
	printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",src->u8[0], src->u8[1],len,last_rssi);

	leds_single_off(LEDS_LED2);
}

/*** MAIN THREAD ***/
PROCESS_THREAD(simple_packet_process, ev, data) {

	static struct etimer et;

	PROCESS_EXITHANDLER(printf("simple_packet_process terminated!\n"));
	PROCESS_BEGIN();

	// set your group's channel
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 12);

	// Initialize NullNet
	nullnet_set_input_callback(broadcast_recv);

	// Define Payload & fill the packet buffer
	static char payload[] = "Hello";
	nullnet_buf = (uint8_t *)payload;
	nullnet_len = sizeof(payload);

	etimer_set(&et, CLOCK_SECOND + 0.1*random_rand()/RANDOM_RAND_MAX); //randomize the sending time a little

	while(1){

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		leds_single_on(LEDS_LED1);

		// Broadcast the message
		NETSTACK_NETWORK.output(NULL);
		
		// for debugging
		printf("Broadcast message sent\r\n");

		// reset the timer
		etimer_reset(&et);

		leds_single_off(LEDS_LED1);
	}

	PROCESS_END();
}

