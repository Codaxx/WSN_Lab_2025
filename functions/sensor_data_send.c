// Contiki-specific includes:
#include "contiki.h"
#include "net/nullnet/nullnet.h"     	// Establish connections.
#include "net/netstack.h"     		    // Wireless-stack definitions.
#include "net/packetbuf.h"				// Use packet buffer.
#include "dev/leds.h"          			// Use LEDs.
//#include "common/temperature-sensor.h"	// Temperature sensor.

// Standard C includes:
#include <stdio.h>      // For printf.
#include "common/saadc-sensor.h"		// Saadc to read Battery Sensor.
#include "common/temperature-sensor.h"
#include "my_sensor.h"

#define BUFFER_SIZE	3
#define DIS_THRES	20
#define	LIGHT_THRES	500
#define	TEM_THRES	10

static uint8_t trans_flag;
static int distance_buffer[BUFFER_SIZE], light_buffer[BUFFER_SIZE], temperature_buffer[BUFFER_SIZE];
static int global_index;
static int distance_av_0, light_av_0, temperature_av_0;
static int distance_av_1, light_av_1, temperature_av_1;

static sensor_data recv_message;

uint16_t get_node_id_from_linkaddr(const linkaddr_t *addr) {
	return ((uint16_t)addr->u8[LINKADDR_SIZE - 2] << 8) | addr->u8[LINKADDR_SIZE - 1];
}  

void distance_av_cal(){
	distance_av_1 = distance_av_0;
	for(int i=0; i<BUFFER_SIZE; i++){
		distance_av_0 += distance_buffer[i];
	}
	distance_av_0 /= BUFFER_SIZE;
}

void light_av_cal(){
	light_av_1 = light_av_0;
	for(int i=0; i<BUFFER_SIZE; i++){
		light_av_0 += light_buffer[i];
	}
	light_av_0 /= BUFFER_SIZE;
}

void temperture_av_cal(){
	temperature_av_1 = temperature_av_0;
	for(int i=0; i<BUFFER_SIZE; i++){
		temperature_av_0 += temperature_buffer[i];
	}
	temperature_av_0 /= BUFFER_SIZE;
}

void recv_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest){
	memcpy(&recv_message, (sensor_data*)data, sizeof(recv_message));

	printf("Received data are from %d:\n\r", get_node_id_from_linkaddr(&recv_message.source));
	printf("batttery: [%d](mV)", recv_message.battery);
	printf("temperature: [%d](C)", recv_message.temperature);
	printf("light: [%d](lux)", recv_message.light_lux);
	printf("distance: [%d](cm)", recv_message.distance);
}

PROCESS(sensor_reader, "Read data from sensors");
AUTOSTART_PROCESSES(&sensor_reader);

PROCESS_THREAD(sensor_reader, ev, data){
	static struct etimer sensor_reading_timer;
	static int light_raw, distance_raw;
	static int light_value, distance_value;
	static int voltage, temperature;
	static sensor_data packet;

	PROCESS_BEGIN();
	etimer_set(&sensor_reading_timer, CLOCK_SECOND);
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 12);
	nullnet_set_input_callback(&recv_callback);

	while(1){
		PROCESS_WAIT_EVENT();
		// read raw data from ADC
		light_raw = saadc_sensor.value(P0_30);
		distance_raw = saadc_sensor.value(P0_31);
		
		// get sensor data
		light_value = get_light_lux(light_raw);
		distance_value = get_distance(distance_raw);
		voltage = get_millivolts(saadc_sensor.value(BATTERY_SENSOR));
		temperature = temperature_sensor.value(0)/4;

		distance_buffer[global_index] = distance_value;
		light_buffer[global_index] = light_value;
		temperature_buffer[global_index] = temperature;
		global_index ++;
		if(global_index>=BUFFER_SIZE){
			distance_av_cal();
			light_av_cal();
			temperture_av_cal();
			global_index = 0;
			trans_flag = 1;
		}
		if(ABS(distance_av_1-distance_av_0) >= DIS_THRES && ABS(light_av_1-light_av_0) >= LIGHT_THRES){
			trans_flag = 1;
		}
		// assign data to packet;
		if(trans_flag){
			packet.type = 3;
			linkaddr_copy(&packet.source, &linkaddr_node_addr);
			packet.light_lux = light_av_0;
			packet.distance = distance_av_0;
			packet.battery = voltage;
			packet.temperature = temperature;

			// transmit data
			nullnet_buf = (uint8_t*) &packet;
			nullnet_len = sizeof(packet);
			NETSTACK_NETWORK.output(NULL);

			trans_flag = 0;
		}
		
		etimer_reset(&sensor_reading_timer);
	}

	PROCESS_END();
}