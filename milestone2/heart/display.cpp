#include "display.h"

TextLCD lcd(p15,p16,p17,p18,p19,p20,TextLCD::LCD16x2);
Timer display_timer;

enum display_state {Display};

void display_thread()
{
    int observation_time;
    display_timer.start();
    display_state state = Display;
    while(1)
    {
        switch (state)
        {
            case Display:
#if TRACE
printf("display: state display\r\n");
#endif
                observation_time = display_timer.read_ms();
#if TRACE
printf("display: observation_time = %d\r\n", observation_time);
#endif
                if (observation_time > OBSERVATION_MIN)
                {
                    
                    //lcd.printf("A: %d bpm\n V: %d bpm\n", Acount/observation_time, Vcount/observation_time);
                    printf("A: %d bpm\r\nV: %d bpm\r\n", (Acount*MS_PER_MINUTE)/observation_time, (Vcount*MS_PER_MINUTE)/observation_time);
                    Acount = 0;
                    Vcount = 0;
                    display_timer.reset();
                }
                break; 
        }
        Thread::yield();
    }
    
}