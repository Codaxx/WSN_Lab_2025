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

   LESSON 2: Custom Payload
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

#define MAX_N 20

typedef struct{
	char name[MAX_N];
	char surname[MAX_N];
	uint8_t tel[MAX_N];
}contact;

//Global variables
static contact tx_contacts;


// Definition of Processes
PROCESS(custom_payload_process, "Lesson 2: Custom Payload");

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

	//Copy the payload of the packetbuffer to a given memory location
	
	//print the content of the memory location
	
}

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                    MAIN PROCESS
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

AUTOSTART_PROCESSES(&custom_payload_process);

PROCESS_THREAD(custom_payload_process, ev, data)
{
	static uint8_t broadcast_time_interval = 3;	// In seconds
	static struct etimer broadcast_table_timer;

	PROCESS_EXITHANDLER(printf("custom_payload_process terminated!\n"))
    PROCESS_BEGIN();

	// set your group's channel
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 26);

	// Change the transmission power
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, 7);

	// Initialize NullNet
	nullnet_set_input_callback(broadcast_recv);

	static int i;

	while(1) //MAIN LOOP
	{
		// Transmitted values
		strcpy(tx_contacts.name,"Contiki");
		strcpy(tx_contacts.surname,"WSN");
		for(i=0;i<10;i++){
			tx_contacts.tel[i]=i;
		}

		leds_single_on(LEDS_LED1);

		//Copy the content of tx_contacts to the buffer.


		NETSTACK_NETWORK.output(NULL);
		leds_single_off(LEDS_LED1);
		printf("CONTACT SENT\n\r\n\r");

		etimer_set(&broadcast_table_timer, CLOCK_SECOND * broadcast_time_interval);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&broadcast_table_timer));
	}
	PROCESS_END();
	return 0;
}

