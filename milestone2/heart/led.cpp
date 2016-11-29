#include "led.h"
DigitalOut LED[4] = {DigitalOut(LED1),DigitalOut(LED2),DigitalOut(LED3),DigitalOut(LED4)};

void flip_led(uint8_t led)
{
	if (led < 0 || led >= NUM_LEDS) {
		//safe_println("Invalid LED: %d", led);
		//while (true);
	}
	LED[led] = !LED[led];
}
void led_thread()
{
    
    
    while(1)
    {
        osEvent evt = Thread::signal_wait(0);
        //Vsignal toggle
        if(evt.value.signals &  SIG_VSIGNAL){
        	flip_led(VPACE_HEART_LED);
        }
        //Asignal toggle
        if(evt.value.signals &  SIG_ASIGNAL){
        	flip_led(APACE_HEART_LED);
        }
        //Vpace toggle
        //Asignal toggle
        if(evt.value.signals &  SIG_VPACE){
        	flip_led(VPACE_LED);
        }
        //Apace toggle
        //Asignal toggle
        if(evt.value.signals &  SIG_APACE){
        	flip_led(APACE_LED);
        }
    }    
    
}