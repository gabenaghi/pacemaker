#include "display.h"

TextLCD lcd(p15,p16,p17,p18,p19,p20,TextLCD::LCD16x2);
Timer display_timer;

enum display_state {Display};

void display_thread()
{
    float observation_time;
    display_timer.start();
    display_state state = Display;
    while(1)
    {
        switch (state)
        {
            case Display:
                switch (rand() % 2)
                {
                    case 0:
                        if (display_timer.read() > OBSERVATION_MIN)
                        {
                            observation_time = display_timer.read();
                            lcd.printf("Acount %3.0f bpm\n Vcount: %3.0f bpm\n",(float)Acount/observation_time, (float)Vcount/observation_time);
                            Acount = 0;
                            Vcount = 0;
                            display_timer.reset();
                        }
                        break;
                        
                    case 1:
                        //do nothing
                        break;   
                }
                break;   
        }
        Thread::yield();
    }
    
}