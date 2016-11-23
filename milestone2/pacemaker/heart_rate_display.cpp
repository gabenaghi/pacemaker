// heart_rate_display.cpp
#include "heart_rate_display.h"
#include "common.h"
#include "TextLCD.h"

TextLCD lcd(p15, p16, p17, p18, p19, p20, TextLCD::LCD16x2);
Ticker heart_rate_timer;

uint32_t A_ticks, V_ticks;
bool print_rate;

void heart_rate_timeout(void)
{
	print_rate = true;
}

void print_heart_rate(void)
{
	print_rate = false;
	lcd.cls();
	lcd.locate(0, 0);
	lcd.printf("A: %d\n", A_ticks / obs_interval);
	lcd.printf("V: %d", V_ticks / obs_interval);
	A_ticks = 0;
	V_ticks = 0;
}

void heart_rate_display_thread(void)
{
	osEvent event;

	A_heart_rate = 0;
	V_heart_rate = 0;
	A_ticks = 0;
	V_ticks = 0;
	print_rate = false;

	heart_rate_timer.attach(&heart_rate_timeout, 1.0 * obs_interval);

	while (true) {
		event = Thread::signal_wait(SIG_ASENSE | SIG_APACE | SIG_VSENSE | SIG_VPACE);
		if (event.value.signals & SIG_ASENSE || event.value.signals & SIG_APACE) {
			A_ticks++;
		}
		if (event.value.signals & SIG_VSENSE || event.value.signals & SIG_VPACE) {
			V_ticks++;
		}
		if (print_rate) {
			print_heart_rate();
		}
	}
}
