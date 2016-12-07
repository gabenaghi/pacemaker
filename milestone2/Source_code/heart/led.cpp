#include "led.h"

void flip_led(uint8_t led)
{
	if (led >= NUM_LEDS) {
		printf("Invalid LED: %d", led);
		return;
	}
	leds[led] = !leds[led];
}

void led_thread()
{
    osEvent evt;
    
    while(1)
    {
#if TRACE
    printf("led\r\n");
#endif
        evt = Thread::signal_wait(0x0);
        //Vsignal toggle
        if(evt.value.signals &  SIG_VSIGNAL){
        	flip_led(VPACE_HEART_LED);
        }
        //Asignal toggle
        if(evt.value.signals &  SIG_ASIGNAL){
        	flip_led(APACE_HEART_LED);
        }
        //Vpace toggle
        if(evt.value.signals &  SIG_VPACE){
        	flip_led(VPACE_LED);
        }
        //Apace toggle
        if(evt.value.signals &  SIG_APACE){
        	flip_led(APACE_LED);
        }
    }    
    
}