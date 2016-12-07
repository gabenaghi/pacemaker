// external_signals.cpp
#include "common.h"
#include "external_signals.h"

#define RESET_TIMEOUT 10 // vpace/apace reset timeout


RtosTimer vget_timer(&reset_vget, osTimerOnce);
RtosTimer aget_timer(&reset_aget, osTimerOnce);

/*
 * Interrupts for resetting Vget and Aget to 0 after RESET_TIMEOUT.
 */

void reset_vget(void const* args)
{
	Vget = 0;
}

void reset_aget(void const* args)
{
	Aget = 0;
}

/*
 * Interrupts for receiving Vget and Aget from the heart.
 */

void vpace_received(void)
{
	global_signal_set(SIG_VPACE);
}

void apace_received(void)
{
	global_signal_set(SIG_APACE);
}


void set_vget(void)
{
	Vget = 1;
	vget_timer.start(RESET_TIMEOUT);
}

void set_aget(void)
{
	Aget = 1;
	aget_timer.start(RESET_TIMEOUT);
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