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

   LESSON 4: Signal Distance
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"      // Establish connections.
#include "net/netstack.h"    		  // Wireless-stack definitions
#include "net/packetbuf.h"
#include "dev/leds.h"          		  // Use LEDs.
#include "dev/button-sensor.h" 		  // User Button
#include "cpu/nrf52840/dev/uart0.h"
// Standard C includes:
#include <stdio.h>        			  // For printf.

#include "dev/serial-line.h"		  // For UART-like I/O over USB.


// Definition of UART special characters. 
// 	Note:	Contiki seems to have its own definitions.
// 			Incompatibility may cause unexpected behavior.
// 	For more details, check usb-serial.h and serial-line.h.
#define START_CHAR                  1
#define DEACTIVATION_CHAR           0x0d //25xxx4
#define END_CHAR                    0x0a //255

// Packet type flags used in wireless transmission.
#define RADIO_PACKET_TYPE_CONFIG_POWER	0
#define RADIO_PACKET_TYPE_INFO_POWER	1

// Packet type flags used in UART/serial transmission.
#define SERIAL_PACKET_TYPE_CONFIG_POWER	0
#define SERIAL_PACKET_TYPE_INFO_POWER	1

// Tx buffer size in bytes.
#define MAX_RF_PAYLOAD_SIZE 	3
#define MAX_USB_PAYLOAD_SIZE	4

// Each packet sent is numbered.
// Numbers will cycle from 1 to 255.
#define MAX_TX_PACKET_NO 255

// While the first value is accessed below, the rest just serves as reference.
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

//--------------------- VARIABLES ---------------------
static int8_t txPower;

static linkaddr_t my_addr; 			// This node's link layer address. Value upon declaration: 
									// {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}

//---------------- FUNCTION PROTOTYPES ----------------

// Handles reception of broadcast messages.
static void broadcast_rx_radio(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);

// Prints the current settings.
static void print_settings(void);

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(txRadio_process, "Sends Tx power value over wireless antenna.");
PROCESS(rxUSB_process, "Receives data from UART/serial (USB).");
AUTOSTART_PROCESSES(&txRadio_process,&rxUSB_process);

//------------------------ PROCESS' THREAD ------------------------

// Configures the wireless antenna and unicast connection.
PROCESS_THREAD(txRadio_process, ev, data) {

    PROCESS_BEGIN();

	static int
		packetNumber = 0,	// Tx packet counter.
		timerInterval = 5;	// In seconds.
	static struct etimer et;

	txPower = outputPower[0];	

	// Configure your team's channel.
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 26);

	// Configure Tx power .
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, txPower);

	my_addr.u8[1] = 0x02;	// Change this value, so that this node will have the desired address and id.
	linkaddr_set_node_addr(&my_addr);

	print_settings();

	/* 
	 * Initialize NullNet.
	 */
	nullnet_set_input_callback(broadcast_rx_radio);

	// We start listening for data over USB.
	//process_start(&rxUSB_process, NULL);

	while(1) {
		etimer_set(&et, CLOCK_SECOND * timerInterval);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		leds_single_on(LEDS_LED1);

		// Increase the RR packet number.
		packetNumber = (packetNumber % MAX_TX_PACKET_NO)+1;

		// Array of bytes we will send over to the destination.
		uint8_t txBuffer[MAX_RF_PAYLOAD_SIZE];

		// Fill the Tx buffer with info and send it.
		txBuffer[0] = RADIO_PACKET_TYPE_INFO_POWER;
		txBuffer[1] = packetNumber;
		txBuffer[2] = txPower;

		// Broadcast power information.
		nullnet_buf = (uint8_t*) txBuffer;
		nullnet_len = MAX_RF_PAYLOAD_SIZE;
	   	NETSTACK_NETWORK.output(NULL);

		leds_single_off(LEDS_LED1);
	}
	PROCESS_END();
}

// Listens for data coming from the USB connection (UART0)
// and prints it.
PROCESS_THREAD(rxUSB_process, ev, data) {
	PROCESS_BEGIN();

	uint8_t *uartRxBuffer;


	/* In this example, whenever data is received from UART0, the
	 * default handler (i.e. serial_line_input_byte) is called.
	 * But how does it work?
	 *
	 * The handler runs in a process that puts all incoming data
	 * in a buffer until it gets a newline or END character
	 * ('\n' and 0x0A are equivalent). Also good to know is that
	 * there is an IGNORE character: 0x0D.
	 *
	 * Once a newline or END is detected, a termination char ('\0')
	 * is appended to the buffer and the process broadcasts a
	 * "serial_line_event_message" along with the buffer contents.
	 *
	 * For more details, check contiki-ng/os/dev/serial-line.c
	 * For an "example", check contiki-ng/os/services/shell/serial-shell.c
	 */
	while(1){
		PROCESS_YIELD();

		if (ev == serial_line_event_message)
		{

			leds_toggle(LEDS_LED2);

			uartRxBuffer = (uint8_t*)data;
			uartRxBuffer += 4;

			// If the received packet is a packet to set the radio power
			if(uartRxBuffer[0] == SERIAL_PACKET_TYPE_CONFIG_POWER) {

				txPower = uartRxBuffer[1];
				// Array of bytes we will send over to the destination.
				uint8_t txBuffer[MAX_RF_PAYLOAD_SIZE];

				printf("[SRC SERIAL]: Set TxPwr (%d)\r\n", txPower);

				// Fill the Tx buffer with info and send it.
				txBuffer[0] = RADIO_PACKET_TYPE_CONFIG_POWER;
				txBuffer[1] = uartRxBuffer[1];

				printf("[FWD to RADIO]\r\n");

				// Change the transmission power. Must be a valid value.
				NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, txPower);

				nullnet_buf = (uint8_t*) txBuffer;
				nullnet_len = MAX_RF_PAYLOAD_SIZE;
	   			NETSTACK_NETWORK.output(NULL);
			}
		}
	}
    PROCESS_END();
}

//---------------- FUNCTION DEFINITIONS ----------------

// Defines the behavior of a broadcast connection upon receiving data.
static void
broadcast_rx_radio(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {


	// Buffers
    static int8_t
    	radioRxBuffer[MAX_RF_PAYLOAD_SIZE],
    	uartTxBuffer[MAX_USB_PAYLOAD_SIZE];
	static int i;					// FOR loop index.

	int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

    printf("[SRC 0x%x%x, BCAST, RSSI %d]: ",
			src->u8[0], src->u8[1], rssi);

	// Get contents of the packet buffer.
    packetbuf_copyto(&radioRxBuffer);

	/* IMPORTANT: The use of switch statements in contiki apps is
	 * discouraged because they *may* disrupt with the structure
	 * of the process and cause unexpected behavior.
	 *
	 * For more details read about "protothread states",
	 * "Local Continuations" and "Duff's Device":
	 *
	 * https://docs.contiki-ng.org/en/develop/doc/programming/Processes-and-events.html
	 * http://senstools.gforge.inria.fr/doku.php?id=os:contiki#processes
	 * http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
	 */

    leds_single_on(LEDS_LED3);
	// Detects the type of the received packet
	if(radioRxBuffer[0] == RADIO_PACKET_TYPE_CONFIG_POWER){
        // ... Set the radio power.
        txPower = radioRxBuffer[1];
        printf("Set TxPwr (%d)\r\n", txPower);

        // Change the transmission power. Must be a valid value.
    	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, txPower);
	}
	else if(radioRxBuffer[0] == RADIO_PACKET_TYPE_INFO_POWER){
        printf("Info: [%d] TxPwr %d.",
        		radioRxBuffer[1],radioRxBuffer[2]);

        uartTxBuffer[0] = RADIO_PACKET_TYPE_INFO_POWER;
        uartTxBuffer[1] = radioRxBuffer[1];
        uartTxBuffer[2] = radioRxBuffer[2];
        uartTxBuffer[3] = rssi;

        printf("[FWD to USB ");
        uart0_writeb(START_CHAR);
        for (i = 0; i < 5; i++){
        	uart0_writeb(uartTxBuffer[i]);
        }
        uart0_writeb(END_CHAR);
        printf("]\r\n");

    } else {
    	printf("RADIO Rx ERR: Unknown packet type\r\n");
    	return;
    }
	leds_single_off(LEDS_LED3);
}


// Prints the current settings.
static void print_settings(void){
	radio_value_t channel;

	NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL,&channel);

	printf("\n-------------------------------------\n");
	printf("Node addr = \t0x%x%x\n",
			linkaddr_node_addr.u8[0],
			linkaddr_node_addr.u8[1]);
	printf("Using radio channel %d\n", channel);
	printf("---------------------------------------\n");
}
