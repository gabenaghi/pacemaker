#include "generator.h"

int SIGNAL_TIME = 5; //arbitrary

enum generator_state{
    waiting, gotV, gotA
};

//AGET AND VGET
DigitalOut Aget(AGET_PIN);
DigitalOut Vget(VGET_PIN);
void Aget_off_fun(void const * n){ Aget = 0;}
void Vget_off_fun(void const * n){ Vget = 0;}
RtosTimer Aget_off(Aget_off_fun, osTimerOnce);
RtosTimer Vget_off(Vget_off_fun, osTimerOnce);

void generator_thread()
{
    osEvent evt;
    generator_state state = waiting;
    
    while (1)
    {
        switch (state)
        {
            case waiting:
                switch (rand() % 3)
                {
                    case 0:
                        evt = Thread::signal_wait(0, ATOMIC_TIME);
                        if (evt.value.signals & SIG_VSIGNAL)
                            state = gotA;
                        break;
                
                    case 1:
                        evt = Thread::signal_wait(0, ATOMIC_TIME);
                        if (evt.value.signals & SIG_ASIGNAL)
                            state = gotV;
                        break;
                    
                    case 2:
                        // do nothing
                        break;
                }
            case gotV:
                Vget = 1;
                Vget_off.start(SIGNAL_TIME);
                state = waiting;
                break;
                
            case gotA:
                Aget = 1;
                Aget_off.start(SIGNAL_TIME);
                state = waiting;
                break;
            
        }
        Thread::yield();
    }
}