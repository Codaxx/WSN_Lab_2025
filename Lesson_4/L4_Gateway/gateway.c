/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2017 Chair of Communication Networks, TUM

   contributors:
   * Thomas Szyrkowiec
   * Mikhail Vilgelm
   * Octavio Rodríguez Cervantes
   * Angel Corona

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   LESSON 3: Gateway
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"    // Establish connections.
#include "net/netstack.h"      		// Wireless-stack definitions
#include "net/packetbuf.h"			// Packet buffer definitions
#include "dev/leds.h"	
#include "common/saadc-sensor.h"
#include "common/temperature-sensor.h"			// Use LEDs.

// Standard C includes:
#include <stdio.h>			// For printf.

//---------------- FUNCTION PROTOTYPES ----------------

// Prints the contents of the Rx buffer.
static void print_buffer_contents(void);

static void broadcast_recv(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);

// Prints the current settings.
static void print_settings(void);

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(gateway_main_process, "Lesson 3: Gateway");
AUTOSTART_PROCESSES(&gateway_main_process);

//------------------------ PROCESS' THREADS ------------------------
PROCESS_THREAD(gateway_main_process, ev, data) {

	PROCESS_BEGIN();

	/*
	* set your group's channel
	*/
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 12);

	print_settings();

	// Initialize NullNet
	nullnet_set_input_callback(broadcast_recv);

	// If all is OK, we can start the other two processes:
	int temp, voltage;
	char to_trans[100] = {0};

	while(1) {
		// Contiki processes cannot start loops that never end.
		temp = temperature_sensor.value(0);
		voltage = (int)(saadc_sensor.value(BATTERY_SENSOR)* 3600UL / 4096);
		memset(to_trans, 0, 100);
		sprintf(to_trans, "\r\nBattery: %4d mV\n\rTemperature: %2d C", voltage, temp);

		nullnet_buf = (uint8_t*) to_trans;
		nullnet_len = strlen(to_trans);
		NETSTACK_NETWORK.output(NULL);


		PROCESS_WAIT_EVENT();
	}
	PROCESS_END();
}

//---------------- FUNCTION DEFINITIONS ----------------

// Defines the behavior of a broadcast connection upon receiving data.
static void
broadcast_recv(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
	leds_single_on(LEDS_LED2);
	printf("[Source 0x%x%x, broadcast, RSSI %d]:\t",
			src->u8[0], src->u8[1],
			(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
	print_buffer_contents();
	leds_single_off(LEDS_LED2);
}

/**
 * Print the received message
 */
static void print_buffer_contents(void){
	char rxBuffer[PACKETBUF_SIZE];
	int rxBytes = 0;

	/*
	 * get the buffered message and message size
	 */
	rxBytes = packetbuf_copyto(&rxBuffer);

	if (rxBytes>0) {
		printf("%s\r\n\n",rxBuffer);
	}

}

// Prints the current settings.
static void print_settings(void) {

	radio_value_t channel = 0;

	NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL,&channel);

	printf("\n------- GATEWAY NODE -------\n");
	printf("Link address  = \t0x%x%x\n",
			linkaddr_node_addr.u8[0],
			linkaddr_node_addr.u8[1]);
	printf("Radio channel = %d\n",channel );
	printf("-----------------------------\n");
}
