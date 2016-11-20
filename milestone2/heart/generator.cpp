#include "generator.h"

enum generator_state{
    waiting, gotV, gotA
};

void generator_thread()
{
    generator_state state = waiting;
    
    while (1)
    {
        switch (state)
        {
            //should these be urgent signals??
            case waiting:
                osEvent evt = Thread::signal_wait(0);
                if (evt.value.signals == SIG_VSIGNAL)
                    break;
                if (evt.value.signals == SIG_ASIGNAL)
                    break;
                
                break;
            
            case gotV:
            
                break;
                
            case gotA:
            
                break;
                
            default:
                // illegal state. how to handle?
                break;
            
        }
    }
}