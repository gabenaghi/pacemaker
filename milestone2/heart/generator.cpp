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
            //should these be urgent signals??
            case waiting:
                evt = Thread::signal_wait(0);
                if (evt.value.signals == SIG_VSIGNAL)
                {
                    state = gotA;
                    break;
                }
                if (evt.value.signals == SIG_ASIGNAL)
                {
                    state = gotV;
                    break;
                }
                //invalid signal
                printf("invalid signal %d in generator->waiting\n", evt.value.signals);
                break;
            
            case gotV:
                Vget = 1 - Vget;
                state = waiting;
                break;
                
            case gotA:
                Aget = 1 - Aget;
                state = waiting;
                break;
                
            default:
                // illegal state.
                printf("Illegal generator state\n");
                break;
            
        }
    }
}