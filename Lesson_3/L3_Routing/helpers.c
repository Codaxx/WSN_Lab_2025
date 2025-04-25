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
   * Donjeta Elshani
   * Onur Ayan
   * Benedikt Hess
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

   LESSON 5: Routing
*/

// turns an LED on and increases the number of packets of this LED ID.

// C includes.
#include <stdio.h>

// Contiki includes.
#include "contiki.h"
#include "dev/leds.h"
#include "net/netstack.h"

// project headers.
#include "helpers.h"

static uint8_t count_on[3] = { 0 }; // counts how many packets with the LED
                                    // IDs 1,2 and 3 are currently
                                    // present.


void turn_on(uint8_t led_id){
	leds_single_on(led_id);

	if ( led_id == LEDS_LED1 ) {
		count_on[0]++;
	} else if ( led_id == LEDS_LED2 ) {
		count_on[1]++;
	} else if ( led_id == LEDS_LED3 ) {
		count_on[2]++;
	}
}


// decreases the the number of packets of an LED ID and turns the LED of i 0.
void turn_off(uint8_t led_id){
	uint8_t index;
	if ( led_id == LEDS_LED1 ) {	
		index = 0;
	} else if ( led_id == LEDS_LED2 ) {	
		index = 1;
	} else if ( led_id == LEDS_LED3 ) {	
		index = 2;
	} else {
		leds_single_off(led_id);
		return;
	}
	count_on[index]--;
	// check if no packets of this LED ID are present any more and turn the LED of if so.
	if ( count_on[index] == 0 ) {
		leds_single_off(led_id);
	}
}


void print_lookup_table(l_table table, uint8_t number_of_nodes){
	uint8_t i;

	printf("Lookup table:\n");
	printf("----------------------------------\n");
	printf("ID| DESTINATION | NEXT_HOP | COST \n");

	for (i = 0; i< number_of_nodes; i++)
	{
	printf  ("%d | 0x%x%x%x%x%x%x%x%x	        |  0x%x%x%x%x%x%x%x%x	   | %d   \n", \
			i, \
			table.dest[i].u8[0],table.dest[i].u8[1],table.dest[i].u8[2],table.dest[i].u8[3],
			table.dest[i].u8[4],table.dest[i].u8[5],table.dest[i].u8[6],table.dest[i].u8[7],
			table.next_hop[i].u8[0],table.next_hop[i].u8[1],table.next_hop[i].u8[2],table.next_hop[i].u8[3],
			table.next_hop[i].u8[4],table.next_hop[i].u8[5],table.next_hop[i].u8[6],table.next_hop[i].u8[7],
			table.cost[i]
			);
	}

	printf("-----------------------------------\n");
}


// Define an LED ID to indicate packets send by this Mote.
uint8_t get_led_id(uint8_t node_id){
	uint8_t led_id;
	printf("\nThis node will forward packets with");

	switch	(node_id)	{
	case 1:	{
		led_id = LEDS_LED1;
		printf(" LED 1.\n\n");
		break;
	}
	case 2:	{
		led_id = LEDS_LED2;
		printf(" LED 2.\n\n");
		break;
	}
	case 3:
		led_id = LEDS_LED3;
		printf(" LED 3.\n\n");
		break;
	default:
		/*
		 * All other cases.
		 * */
		led_id = LEDS_LED4;
		printf(" LED 4.\n\n");
	}
	return led_id;
}


// Prints an address.
void print_address(linkaddr_t addr){
	printf("\n-------------------------------------\n");
	printf("Addr = \t0x%x%x%x%x%x%x%x%x\n",
			addr.u8[0],
			addr.u8[1],
			addr.u8[2],
			addr.u8[3],
			addr.u8[4],
			addr.u8[5],
			addr.u8[6],
			addr.u8[7]);
	printf("---------------------------------------\n");
}


// Prints the current settings.
void print_settings(void){
	radio_value_t channel;

	NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL,&channel);

	printf("\n-------------------------------------\n");
	printf("Addr = \t0x%x%x%x%x%x%x%x%x\n",
			linkaddr_node_addr.u8[0],
			linkaddr_node_addr.u8[1],
			linkaddr_node_addr.u8[2],
			linkaddr_node_addr.u8[3],
			linkaddr_node_addr.u8[4],
			linkaddr_node_addr.u8[5],
			linkaddr_node_addr.u8[6],
			linkaddr_node_addr.u8[7]);
	printf("Using radio channel %d\n", channel);
	printf("---------------------------------------\n");
}


// Checks if the loaded RIME address is all zeroes.
void check_for_invalid_addr(void){
	// Boolean flag to check invalid address.
	static int iAmError = 0;

	// All-zeroes address. 
	static linkaddr_t errAddr;
	errAddr.u8[0] = 0;
	errAddr.u8[1] = 0;

	// Check if this mote got an invalid address.
	iAmError = linkaddr_cmp(&errAddr, &linkaddr_node_addr);

	// Check if this node got an invalid address
	// according to lesson specification.
	if( ((linkaddr_node_addr.u8[1]& 0xFF) > TOTAL_NODES))
		iAmError = 0x01;

	// Turn ON all LEDs if we loaded an invalid address.
	if(iAmError){
		printf("\nLoaded an invalid RIME address (0x%x%x)! "
				"Reprogramm the device.\n\n",
				linkaddr_node_addr.u8[0],
				linkaddr_node_addr.u8[1]);

		/* Freezes the app here. Reset needed. */
		while (1){
			leds_single_on(LEDS_LED1);
		}
	}
}


uint8_t calculate_destination(uint8_t id, uint8_t network_size){
	return (id + network_size - 2) % network_size + 1;
}
