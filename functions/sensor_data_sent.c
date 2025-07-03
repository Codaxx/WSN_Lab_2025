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

static sensor_data recv_message;

void recv_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest){
	memcpy(&recv_message, (sensor_data*)data, sizeof(recv_message));

	printf("Received data are:\n\r");
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

		// assign data to packet;
		packet.type = 3;
		linkaddr_copy(&packet.source, &linkaddr_node_addr);
		packet.light_lux = light_value;
		packet.distance = distance_value;
		packet.battery = voltage;
		packet.temperature = temperature;

		// transmit data
		nullnet_buf = (uint8_t*) &packet;
		nullnet_len = sizeof(packet);
		NETSTACK_NETWORK.output(NULL);

		etimer_reset(&sensor_reading_timer);
	}

	PROCESS_END();
}
