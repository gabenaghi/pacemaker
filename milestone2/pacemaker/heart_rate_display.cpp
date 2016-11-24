// heart_rate_display.cpp
#include "heart_rate_display.h"
#include "common.h"
#include "TextLCD.h"

TextLCD lcd(p9, p10, p11, p12, p13, p14, TextLCD::LCD16x2);
RtosTimer heart_rate_timer(&heart_rate_timeout);

uint32_t A_ticks, V_ticks;

void heart_rate_timeout(void const* args)
{
	lcd.cls();
	lcd.locate(0, 0);
	lcd.printf("A: %d\n", (uint8_t) (1.0 * A_ticks / obs_interval));
	lcd.printf("V: %d", (uint8_t) (1.0 * V_ticks / obs_interval));
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
		if (event.value.signals & SIG_AGET || event.value.signals & SIG_APACE) {
			A_ticks++;
		}
		if (event.value.signals & SIG_VGET || event.value.signals & SIG_VPACE) {
			V_ticks++;
		}
		clear_own_signals(T_HEART_RATE_DISPLAY);
		
	}
}
