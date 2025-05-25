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

   LESSON 5: Flooding (broadcast)
*/

// Contiki-specific includes: 
#include "contiki.h"
#include "net/nullnet/nullnet.h"	// Establish connections.
#include "net/packetbuf.h"
#include "net/netstack.h"      // Wireless-stack definitions.
#include "dev/leds.h"          // Use LEDs.
#include "net/linkaddr.h"

// Standard C includes:
#include <stdio.h>      // For printf.
#include <string.h>		// For strlen.

char recv_message[PACKETBUF_SIZE];
int size=0;

// Provide a function which fowards the message "forward_msg()"
/*** YOUR CODE HERE ***/


//---------------------------------------------------------------------------
// Defines the behavior of a connection upon receiving data.
static void
broadcast_recv(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) 
{
	leds_single_on(LEDS_LED2);

	// Get packet's RSSI.
	signed short last_rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

	printf("Broadcast message received from 0x%x%x, len: %d, [RSSI %d]\n\r", src->u8[0], src->u8[1], len, last_rssi);

	// Copy the payload of the packetbuffer to a given memory location.
	/*** YOUR CODE HERE ***/
	memset(recv_message, 0 ,PACKETBUF_SIZE);
	packetbuf_copyto(recv_message);
	printf("Message is: {%s}\r\n", recv_message);

	// Forward the message
	/*** YOUR CODE HERE ***/
	nullnet_buf = (uint8_t*) recv_message;
	nullnet_len = PACKETBUF_SIZE;
	NETSTACK_NETWORK.output(NULL);

	leds_single_off(LEDS_LED2);
}
//---------------------------------------------------------------------------
static void 
check_for_invalid_addr(void) 
{
	// Boolean flag to check invalid address.
	static int iAmError = 0;

	// All-zeroes address.
	static linkaddr_t errAddr;
	errAddr.u8[0] = 0;
	errAddr.u8[1] = 0;

	// Check if this mote got an invalid address.
	iAmError = linkaddr_cmp(&errAddr, &linkaddr_node_addr);

	// Turn ON all LEDs if we loaded an invalid address.
	if(iAmError){
		printf("\nLoaded an invalid RIME address (0x%x%x)! "
				"Reset the device.\n\n",
				linkaddr_node_addr.u8[0],
				linkaddr_node_addr.u8[1]);

		// Freezes the app here. Reset needed.
		while (1){
			leds_single_on(LEDS_LED1);
		}
	}
}
//---------------------------------------------------------------------------
// Prints the current settings. 
static void 
print_settings(void)
{
	radio_value_t channel;

	NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL, &channel);

	printf("\n-------------------------------------\n");
	printf("Node addr = \t0x%x%x\n",
			linkaddr_node_addr.u8[0],
			linkaddr_node_addr.u8[1]);
	printf("Using radio channel %d\n", channel);
	printf("---------------------------------------\n");
}
//---------------------------------------------------------------------------
//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(flooding_process, "Lesson 5: Flooding");
AUTOSTART_PROCESSES(&flooding_process);

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD(flooding_process, ev, data) {

	PROCESS_EXITHANDLER(printf("flooding_process terminated!\n");)
	PROCESS_BEGIN();

	static uint8_t timer_interval = 3;	// In seconds.

	static struct etimer et;

	static char my_message[] = "Hello";

	// Configure your team's channel (11 - 26).
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,12);

	print_settings();
	check_for_invalid_addr();

	// Initialize NullNet.
	nullnet_set_input_callback(broadcast_recv);

	while(1) {
		etimer_set(&et, CLOCK_SECOND * timer_interval);

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		leds_single_on(LEDS_LED1);

		// Send message.
		nullnet_buf = (uint8_t*) my_message;
		nullnet_len = strlen(my_message);
		NETSTACK_NETWORK.output(NULL);	// Address NULL means we send a broadcast.

		printf("Broadcast message sent.\r\n");

		leds_single_off(LEDS_LED1);
	}
	PROCESS_END();
}

