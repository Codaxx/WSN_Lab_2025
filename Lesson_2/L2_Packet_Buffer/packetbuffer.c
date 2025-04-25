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

   LESSON 1: Blinking LEDs
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

static uint8_t my_packetbuffer[PACKETBUF_SIZE];

// Definition of Processes
PROCESS(packet_buffer_process, "Lesson 2: Packet Buffer");

//Function prototypes
static void print_packetbuffer();
static void copy_and_print_packetbuffer();


/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   	   	   	   	   	   	   	  BROADCAST CONNECTION
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

// Defines the behavior of a connection upon receiving data.
void broadcast_recv(const void *data, uint16_t len,
  					const linkaddr_t *src, const linkaddr_t *dest) 
{
	leds_single_on(LEDS_LED2);

	/* Get packet's RSSI */
	signed short last_rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

	printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",src->u8[0], src->u8[1],len,last_rssi);

	printf("RECEIVED - Copy from Input Callback \n\r");
	char payload[20];
	memcpy(payload, data, len);
	printf("%s\n\r", payload);

	printf("RECEIVED - Copy \n\r");
	copy_and_print_packetbuffer();

	printf("RECEIVED - Print \n\r");
	print_packetbuffer();

	leds_single_off(LEDS_LED2);
}

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                    MAIN PROCESS
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

AUTOSTART_PROCESSES(&packet_buffer_process);

PROCESS_THREAD(packet_buffer_process, ev, data)
{
	static struct etimer et;

	PROCESS_EXITHANDLER(printf("packet_buffer_process terminated!\n"))

	PROCESS_BEGIN();

	// set your group's channel
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 26);

	// Change the transmission power
	//NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, (radio_value_t) RADIO_TXPOWER_TXPOWER_0dBm);

	// Initialize NullNet
	nullnet_set_input_callback(broadcast_recv);

	// Define Payload & fill the packet buffer
	static char payload[] = "Contiki";
	nullnet_buf = (uint8_t *)payload;
	nullnet_len = sizeof(payload);

	etimer_set(&et, CLOCK_SECOND);

	while(1) //MAIN LOOP
	{

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		leds_single_on(LEDS_LED1);

		// fill the packet buffer
		nullnet_buf = (uint8_t *)payload;
		nullnet_len = sizeof(payload);

		// send the message
		NETSTACK_NETWORK.output(NULL);

		// for debugging
		printf("Broadcast message sent\n\r");

		print_packetbuffer();

		// reset the timer
		etimer_reset(&et);

		leds_single_off(LEDS_LED1);
	}
	PROCESS_END();

}


/* ----------------------- Exercise prototypes ----------------------- */

static void copy_and_print_packetbuffer(){
	//copy the payload of the packetbuffer to a given memory location

	//print the content of the memory location

}

static void print_packetbuffer(){
	//retrieve the pointer to the payload and the length of it

	//use the retrieved information to print the content of the payload

}
