#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-hal.h"
#include "dev/serial-line.h"

PROCESS(first_process, "Main process of the first WSN lab application");
AUTOSTART_PROCESSES(&first_process);

PROCESS_THREAD(first_process, ev, data){
    PROCESS_BEGIN();
    serial_line_init();
    leds_off(LEDS_ALL);
    static button_hal_button_t* btn;

    while(1){
        PROCESS_WAIT_EVENT();

        // button event
        if(ev == button_hal_press_event){
            btn = (button_hal_button_t*) data;
            printf("Press event (%s)\r\n", BUTTON_HAL_GET_DESCRIPTION(btn));

            // if the first button was pressed, turned LED1 on
            if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ZERO)){
                printf("Turn on LED 1.\r\n");
                leds_single_on(LEDS_LED1);
            }
            // if the second button was pressed, toggled LED2
            if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ONE)){
                printf("Toggle LED 2.\r\n");
                leds_single_toggle(LEDS_LED2);
            }
            // if the forth button was pressed, turned LED 1, 2, 3 on
            if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_THREE)){
                leds_on(LEDS_NUM_TO_MASK(LEDS_LED1) | LEDS_NUM_TO_MASK(LEDS_LED2) | LEDS_NUM_TO_MASK(LEDS_LED3));
            }

        }else if(ev == button_hal_release_event){
            btn = (button_hal_button_t*) data;
            printf("Release event (%s)\r\n", BUTTON_HAL_GET_DESCRIPTION(btn));

            // if the first button was released, turned LED1 off
            if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ZERO)){
                printf("Turn off LED 1.\r\n");
                leds_single_off(LEDS_LED1);
            }
            // if the third button was released, turned all LEDs off
            // And reset the seconds counter
            if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_TWO)){
                leds_off(LEDS_ALL);
            }

        }else if(ev == button_hal_periodic_event){
            // if the third button was held for one second,
            // the second counter would add 1. when the counter is larger than 3,
            // LED3 would be turned on.
            btn = (button_hal_button_t*) data;
            if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_TWO)){
                if(btn->press_duration_seconds >= 3){
                    leds_single_on(LEDS_LED3);
                }
            }
        }

        // serial event
        else if(ev == serial_line_event_message){
            printf("data is %s\r\n", (char*)data);
            if(strncmp("cmd:led1.on", (char*)data, 11)==0){
                leds_single_on(LEDS_LED1);
            }
            if(strncmp("cmd:leds.off", (char*)data, 11)==0){
                leds_single_off(LEDS_LED2);
            }
        }
    }

    PROCESS_END();
}
