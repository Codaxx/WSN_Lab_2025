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

   LESSON 3: External Sensors.
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"      // Establish connections.
#include "net/netstack.h"             // Wireless-stack definitions.
#include "net/packetbuf.h"			  // Reading packet buffer.
#include "dev/leds.h"     		  	  // Use LEDs.
#include "dev/button-hal.h" 		  // User Button.
#include "common/saadc-sensor.h"	  // Reading of external sensors' values.

// Standard C includes:
#include <stdio.h>      // For printf.
#include <string.h>

// Reading frequency in seconds.
#define TEMP_READ_INTERVAL CLOCK_SECOND*1

// Defines a custom event to be used as inter-process communication.

/*** CONNECTION DEFINITION***/

/**
 * Callback function for received packet processing.
 *
 */
static void broadcast_recv(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {

	leds_single_on(LEDS_LED2);

	int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

	printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",src->u8[0], src->u8[1],len,rssi);

	leds_single_off(LEDS_LED2);
}

/**
 * CONNECTION DEFINITION END 
*/

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS (ext_sensors_process, "External Sensors process");
AUTOSTART_PROCESSES (&ext_sensors_process);


//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD (ext_sensors_process, ev, data) {

	/* variables to be used */
	static struct etimer temp_reading_timer;
	static uint16_t p0_30_value, p0_31_value;

	PROCESS_BEGIN ();


	printf("\r\nnrf52840 external sensors");
	printf("\r\n====================================");

	/*
	 * set your group's channel
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,26);

	// Initialize NullNet
	nullnet_set_input_callback(broadcast_recv);

	etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);

	while (1) {

		PROCESS_WAIT_EVENT();  // let process continue

		/* If timer expired, print sensor readings */
	    if(ev == PROCESS_EVENT_TIMER) {

	    	leds_single_on(LEDS_LED3);		

			/*
	    	 * Read ADC values. Data is in the 12 MSBs
	    	 */
	    	p0_30_value = saadc_sensor.value(P0_30);
	    	p0_31_value = saadc_sensor.value(P0_31);

	    	/*
	    	 * Print Raw values
	    	 */

	    	printf("\r\nP0.30 value [Raw] = %d", p0_30_value);
	        printf("\r\nP0.31 value [Raw] = %d", p0_31_value);

    		leds_single_off(LEDS_LED3);

    		etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);
	    }
    }

	PROCESS_END ();
}



