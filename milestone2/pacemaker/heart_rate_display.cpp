// heart_rate_display.cpp
#include "heart_rate_display.h"
#include "common.h"

RtosTimer heart_rate_timer(&heart_rate_timeout);

volatile uint32_t A_ticks, V_ticks;

void heart_rate_timeout(void const* args)
{
	lcd.locate(0, 0);
	lcd.printf("A: %d, V: %d   ", A_ticks, V_ticks);
	A_ticks = 0;
	V_ticks = 0;
}

void print_heart_rate(void)
{

}

void heart_rate_display_thread(void)
{
	
	osEvent event;
	
	A_ticks = 0;
	V_ticks = 0;
	
	heart_rate_timer.start(100 * obs_interval);
	
	while (true) {
		event = Thread::signal_wait(0);
		if (event.value.signals & SIG_AGET) {
			A_ticks++;
		}
		if (event.value.signals & SIG_VGET) {
			V_ticks++;
		}
		clear_own_signals(T_HEART_RATE_DISPLAY);
		
	}
}

