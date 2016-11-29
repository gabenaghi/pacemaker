// external_signals.cpp
#include "common.h"
#include "external_signals.h"

#define RESET_TIMEOUT 0.01f // vpace/apace reset timeout


RtosTimer vget_timer(&reset_vget);
RtosTimer aget_timer(&reset_aget);

/*
 * Interrupts for receiving Vget and Aget from the heart.
 */

void vpace_received(void)
{
	global_signal_set(SIG_VPACE);
	//flip_led(LED_VGET);
	pc.printf("Received VPACE");
}

void apace_received(void)
{
	global_signal_set(SIG_APACE);
	//flip_led(LED_AGET);
	pc.printf("Received Aget");
}

/*
 * Interrupts for resetting Vget and Aget to 0 after RESET_TIMEOUT.
 */

void reset_vget(void const* args)
{
	Vget = 0;
	vget_timer.stop();
}

void reset_aget(void const* args)
{
	Aget = 0;
	aget_timer.stop();
}

void set_vget(void)
{
	Vget = 1;
	vget_timer.start(RESET_TIMEOUT);
	//flip_led(LED_VPACE);
}

void set_aget(void)
{
	Aget = 1;
	aget_timer.start(RESET_TIMEOUT);
	//flip_led(LED_APACE);
}

void external_signals_thread(void) 
{
	osEvent event;
	Vpace.rise(&vpace_received);
	Apace.rise(&apace_received);

	while (true) {
		event = Thread::signal_wait(0);
		if (event.value.signals & SIG_VSIGNAL) {
			set_vget();
		} else if (event.value.signals & SIG_ASIGNAL) {
			set_aget();
		}
		clear_own_signals(T_EXTERNAL);
	}
}