#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


// PHY LAYER PARAMETERS
#define CHANNEL 26
#define TX_POWER (radio_value_t) RADIO_TXPOWER_TXPOWER_Pos4dBm

// MEASUREMENT PARAMETERS (Inter-packet-time: 128 ticks = 1 second):
#define MAX_MESSAGE_LENGTH 	100		//  Message length (values btw 8-100)
#define MAX_NUM_TX_PACKETS	200		//  Maximum total number of transmitted packets.


// Adjust button configurations to test contention-based MAC

#define INTER_PACKET_TIME_BUTTON_1  32   // ~0.25 seconds
#define TOTAL_TX_PACKETS_BUTTON_1   50

#define INTER_PACKET_TIME_BUTTON_2   8    // ~0.06 seconds
#define TOTAL_TX_PACKETS_BUTTON_2   100

#define INTER_PACKET_TIME_BUTTON_3    2   // ~0.016 seconds
#define TOTAL_TX_PACKETS_BUTTON_3   200

#define INTER_PACKET_TIME_BUTTON_4    0    // No waiting time
#define TOTAL_TX_PACKETS_BUTTON_4   200



#endif /* PROJECT_CONF_H_ */
