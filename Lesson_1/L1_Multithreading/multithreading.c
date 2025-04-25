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
PROCESS(multithreading_proccess, "Lesson 1: Multithreading");
AUTOSTART_PROCESSES(&multithreading_proccess);

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD(multithreading_proccess, ev, data){

	static struct etimer timerLED1, timerLED2, timerLED3;

	PROCESS_BEGIN();

	printf("Timers set!\r\n ");
	/*
	 * Set timers
	 */
	etimer_set(&timerLED1, CLOCK_SECOND);
	etimer_set(&timerLED2, CLOCK_SECOND/2);
	etimer_set(&timerLED3, CLOCK_SECOND/4);

	while(1) {
		PROCESS_WAIT_EVENT();
		if(etimer_expired(&timerLED1)) {
			printf("Timer expired for LED 1...\r\n");
			leds_single_toggle(LEDS_LED1);
			etimer_reset(&timerLED1);
		}
		else if(etimer_expired(&timerLED2)) {
			printf("Timer expired for LED 2...\r\n");
			leds_single_toggle(LEDS_LED2);
			etimer_reset(&timerLED2);
		}
		else if(etimer_expired(&timerLED3)) {
			printf("Timer expired for LED 3...\r\n");
			leds_single_toggle(LEDS_LED3);
			etimer_reset(&timerLED3);
		}
	}
	PROCESS_END();
}

