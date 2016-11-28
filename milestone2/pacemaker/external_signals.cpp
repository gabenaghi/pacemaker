// external_signals.cpp
#include "common.h"
#include "external_signals.h"

#define RESET_TIMEOUT 0.01f // vpace/apace reset timeout

RtosTimer vpace_timer(&reset_vpace);
RtosTimer apace_timer(&reset_apace);

void flip_led(uint8_t led)
{
	if (led < 0 || led >= NUM_LEDS) {
		safe_println("Invalid LED: %d", led);
		while (true);
	}
	leds[led] = !leds[led];
}

/*
 * Interrupts for receiving Vget and Aget from the heart.
 */

void vget_received(void)
{
	global_signal_set(SIG_VGET);
	flip_led(LED_VGET);
	safe_println("Received Vget");
}

void aget_received(void)
{
	global_signal_set(SIG_AGET);
	flip_led(LED_AGET);
	safe_println("Received Aget");
}

/*
 * Interrupts for resetting Vpace and Apace to 0 after RESET_TIMEOUT.
 */

void reset_vpace(void const* args)
{
	Vpace = 0;
	vpace_timer.stop();
}

void reset_apace(void const* args)
{
	Apace = 0;
	apace_timer.stop();
}

void set_vpace(void)
{
	Vpace = 1;
	vpace_timer.start(RESET_TIMEOUT);
	flip_led(LED_VPACE);
}

void set_apace(void)
{
	Apace = 1;
	apace_timer.start(RESET_TIMEOUT);
	flip_led(LED_APACE);
}

void external_signals_thread(void) 
{
	osEvent event;
	Vget.rise(&vget_received);
	Aget.rise(&aget_received);

	while (true) {
		event = Thread::signal_wait(0);
		if (event.value.signals & SIG_VPACE) {
			set_vpace();
		} else if (event.value.signals & SIG_APACE) {
			set_apace();
		}
		clear_own_signals(T_EXTERNAL_SIGNALS);
	}
}

