// heart_rate_display.cpp
#include "heart_rate_display.h"
#include "common.h"
#include "TextLCD.h"

TextLCD lcd(LCD_PIN_0, LCD_PIN_1, LCD_PIN_2, LCD_PIN_3, LCD_PIN_4, LCD_PIN_5, TextLCD::LCD16x2);
RtosTimer heart_rate_timer(&heart_rate_timeout);

volatile uint32_t A_ticks, V_ticks;

void heart_rate_timeout(void const* args)
{
	lcd.cls();
	lcd.locate(0, 0);
	lcd.printf("A: %d\n", A_ticks);
	lcd.printf("V: %d", V_ticks);
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

