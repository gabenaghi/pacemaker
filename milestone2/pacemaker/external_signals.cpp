// external_signals.cpp
#include "common.h"
#include "external_signals.h"

#define RESET_TIMEOUT 0.01f // vpace/apace reset timeout

Ticker vpace_timer;
Ticker apace_timer;

/*
 * Interrupts for receiving Vget and Aget from the heart.
 */

void vget_received(void)
{
	global_signal_set(SIG_VGET);
}

void aget_received(void)
{
	global_signal_set(SIG_AGET);
}

/*
 * Interrupts for resetting Vpace and Apace to 0 after RESET_TIMEOUT.
 */

void reset_vpace(void)
{
	Vpace = 0;
	vpace_timer.attach(NULL, 0.0);
}

void reset_apace(void)
{
	Apace = 0;
	apace_timer.attach(NULL, 0.0);
}

void external_signals_thread(void) 
{
	osEvent event;
	Vget.rise(&vget_received);
	Aget.rise(&aget_received);

	while (true) {
		event = Thread::signal_wait(SIG_VPACE | SIG_APACE);
		if (event.value.signals & SIG_VPACE) {
			Vpace = 1;
			vpace_timer.attach(&reset_vpace, RESET_TIMEOUT);
		} else if (event.value.signals & SIG_APACE) {
			Apace = 1;
			apace_timer.attach(&reset_apace, RESET_TIMEOUT);
		}
		threads[T_EXTERNAL_SIGNALS]->signal_clr(0xFFFFFFFF);
	}
}
