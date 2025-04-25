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

   LESSON 3: Temperature Sensor.
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"     	// Establish connections.
#include "net/netstack.h"     		    // Wireless-stack definitions.
#include "net/packetbuf.h"				// Use packet buffer.
#include "dev/leds.h"          			// Use LEDs.
#include "common/temperature-sensor.h"	// Temperature sensor.
#include "common/saadc-sensor.h"		// Saadc to read Battery Sensor.
// Standard C includes:
#include <stdio.h>      // For printf.

// Reading frequency in seconds.
#define TEMP_READ_INTERVAL CLOCK_SECOND*1

/**
 * Function to convert raw value of ADC to mV
 * 
 */
static int saadc_to_millivolts(uint16_t saadc_value);
static linkaddr_t my_addr; 			// This node's link layer address. Value upon declaration: 
									// {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}

/*** CONNECTION DEFINITION***/

/**
 * Callback function for received packet processing.
 *
 */
static void broadcast_recv(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {

	leds_single_on(LEDS_LED2);

	// Get packet's RSSI.
	signed short last_rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);

	printf("Broadcast message received from 0x%x%x, len: %d, [RSSI %d]\n", src->u8[0], src->u8[1], len, last_rssi);

	leds_single_off(LEDS_LED2);
}

/*** CONNECTION DEFINITION END ***/

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS (on_board_sensors_process, "Lesson 3: On-Board Sensors");
AUTOSTART_PROCESSES (&on_board_sensors_process);

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD (on_board_sensors_process, ev, data) {

	my_addr.u8[1] = 0x02;	// Change this value, so that this node will have the desired address and id.
	linkaddr_set_node_addr(&my_addr);
	
	/* variables to be used */
	static struct etimer temp_reading_timer;

	PROCESS_BEGIN ();

	printf("\r\nnrf52840 on-board sensors");
	printf("\r\n====================================");

	/*
	 * set your group's channel
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,26);

	/* 
	 * Initialize NullNet.
	 */
	nullnet_set_input_callback(broadcast_recv);

	etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);

	while (1) {

		PROCESS_WAIT_EVENT();  // let process continue

		/* If timer expired, print sensor readings */
	    if(ev == PROCESS_EVENT_TIMER) {

	    	leds_single_on(LEDS_LED1);
			printf("\r\nMy Battery Voltage [VDD] = %d mV", saadc_to_millivolts(saadc_sensor.value(BATTERY_SENSOR)));
    		leds_single_off(LEDS_LED1);

    		etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);
	    }

    }

	PROCESS_END ();
}

static int saadc_to_millivolts(uint16_t saadc_value){
	// V_ref = 600mV
	// gain = 1/6
	// resolution = 2^12 = 4096
	// converted = raw * (V_ref / gain) / resultion
	return (int)((saadc_value * 3600UL) / 4096);
}
