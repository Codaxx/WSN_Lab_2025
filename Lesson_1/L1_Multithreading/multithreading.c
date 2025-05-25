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

   LESSON 1: Multithreading
*/

// Contiki-specific includes:
#include "contiki.h"
#include "dev/leds.h" // Enables use of LEDs.

// Standard C includes:
#include <stdio.h>    // For printf.


//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(led1_blink_1, "led1 blinks each 1 second");
PROCESS(led2_0_5, "led2 blinks each 0.5 seconds");
PROCESS(led3_0_25, "led3 blinks each 0.25 seconds");

AUTOSTART_PROCESSES(&led1_blink_1, &led2_0_5, &led3_0_25);


PROCESS_THREAD(led1_blink_1, ev, data){
	PROCESS_BEGIN();

	static struct etimer et;
	etimer_set(&et, CLOCK_SECOND);
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		leds_single_toggle(LEDS_LED1);
		etimer_reset(&et);
	}

	PROCESS_END();
}

PROCESS_THREAD(led2_0_5, ev, data){
	PROCESS_BEGIN();

	static struct etimer et;
	etimer_set(&et, CLOCK_SECOND/2);
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		leds_single_toggle(LEDS_LED2);
		etimer_reset(&et);
	}

	PROCESS_END();
}


PROCESS_THREAD(led3_0_25, ev, data){
	PROCESS_BEGIN();

	static struct etimer et;
	etimer_set(&et, CLOCK_SECOND/4);
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		leds_single_toggle(LEDS_LED3);
		etimer_reset(&et);
	}

	PROCESS_END();
}

