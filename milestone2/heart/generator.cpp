#include "generator.h"

enum generator_state{
    waiting, gotV, gotA
};

//AGET AND VGET
DigitalOut Aget(AGET_PIN);
DigitalOut Vget(VGET_PIN);

void generator_thread()
{
    generator_state state = waiting;
    osEvent evt;
    
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
                Vget = 1 - Vget;
                state = waiting;
                break;
                
            case gotA:
                Aget = 1 - Aget;
                state = waiting;
                break;
            
        }
    }
}