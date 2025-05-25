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

   LESSON 2: TRansmission Power
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"	// Establish connections.
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "lib/random.h"
#include "dev/leds.h"
#include "dev/button-hal.h"

// Standard C includes:
#include <string.h>
#include <stdio.h>
#include <stdint.h>


static radio_value_t outputPower[12] = {
	RADIO_TXPOWER_TXPOWER_Pos8dBm,
	RADIO_TXPOWER_TXPOWER_Pos6dBm,
	RADIO_TXPOWER_TXPOWER_Pos4dBm,
	RADIO_TXPOWER_TXPOWER_Pos2dBm,
	RADIO_TXPOWER_TXPOWER_0dBm,
	RADIO_TXPOWER_TXPOWER_Neg4dBm,
	RADIO_TXPOWER_TXPOWER_Neg8dBm,
	RADIO_TXPOWER_TXPOWER_Neg12dBm,
	RADIO_TXPOWER_TXPOWER_Neg16dBm,
	RADIO_TXPOWER_TXPOWER_Neg20dBm,
	RADIO_TXPOWER_TXPOWER_Neg30dBm,
	RADIO_TXPOWER_TXPOWER_Neg40dBm
};

static uint8_t powerIndex= 4;    //change transmission power here

/**
 * Callback function for received packet processing.
 *
 */
void broadcast_recv(const void *data, uint16_t len,
  					const linkaddr_t *src, const linkaddr_t *dest)  
{
	leds_single_on(LEDS_LED2);

	/* Get packet's RSSI */
	signed short last_rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);


	printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",src->u8[0], src->u8[1],len,last_rssi);

	leds_single_off(LEDS_LED2);
}

/*** MAIN PROCESS DEFINITION ***/
PROCESS(transmission_power_process, "Lesson 2: Transmission Power");
// PROCESS(button_process, "Lesson 2: Change power");
AUTOSTART_PROCESSES(&transmission_power_process);

/*** MAIN THREAD ***/
PROCESS_THREAD(transmission_power_process, ev, data) {

	static struct etimer et;
	radio_value_t powerSet=0;

	PROCESS_EXITHANDLER(printf("transmission_power_process terminated!\n"));
	PROCESS_BEGIN();

	/*
	 * set your group's channel
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 12);

	/*
	 * Change the transmission power here
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, outputPower[powerIndex]);

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

		// send the message
		NETSTACK_NETWORK.output(NULL);

		// for debugging
		NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_TXPOWER, &powerSet);
		printf("Broadcast message sent with power: %d dBm\r\n", (int8_t)powerSet);

		// reset the timer
		etimer_reset(&et);

		leds_single_off(LEDS_LED1);
	}

	PROCESS_END();
}
