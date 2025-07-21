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

   LESSON 1: Timers and Threads
*/


// Contiki-specific includes:
#include "contiki.h"
#include "dev/leds.h"			// Enables use of LEDs

// Standard C includes:
#include <stdio.h>		// For printf


PROCESS(timers_and_threads_process, "Lesson 1: Timers and Threads");
PROCESS(led2_blink, "led 2 is made to blink");
PROCESS(exit_process_2, "exits the process 2");

AUTOSTART_PROCESSES(&timers_and_threads_process, &led2_blink, &exit_process_2);



//------------------------ PROCESS' THREAD ------------------------
static struct ctimer ct;

static void callback(void *ptr){
    ctimer_reset(&ct);
    leds_single_toggle(LEDS_LED1);
}

PROCESS_THREAD(timers_and_threads_process, ev, data) {

	PROCESS_EXITHANDLER( printf("main_process terminated!\n"); )
    PROCESS_BEGIN();


    ctimer_set(&ct, CLOCK_SECOND*2, callback, NULL);

    while (1){
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}

PROCESS_THREAD(led2_blink, ev, data){
    PROCESS_BEGIN();
    PROCESS_EXITHANDLER(printf("led2 blinking process has terminated\n\r"));

    static struct etimer et;

    etimer_set(&et, CLOCK_SECOND);

    while(1){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        leds_single_toggle(LEDS_LED2);
        etimer_reset(&et);
    }

    PROCESS_END();
}

PROCESS_THREAD(exit_process_2, ev, data){
    PROCESS_BEGIN();

    static struct etimer et;

    etimer_set(&et, CLOCK_SECOND*5);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    process_exit(&led2_blink);
    etimer_stop(&et);

    PROCESS_END();
}
