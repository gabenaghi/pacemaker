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
#if TRACE
printf("generator: state waiting\r\n");
#endif
                evt = Thread::signal_wait(0, SIG_TIMEOUT);
                if (evt.value.signals & SIG_VSIGNAL)
                    state = gotV;
#if TRACE
printf("generator: got vsignal\r\n");
#endif
                if (evt.value.signals & SIG_ASIGNAL)
                    state = gotA;
#if TRACE
printf("generator: got asignal\r\n");
#endif
                break;
            case gotV:
#if TRACE
    printf("generator: state gotV\r\n");
#endif
                Vget = 1;
                Vget_off.start(SIGNAL_TIME);
                Vcount++;
                state = waiting;
                break;
                
            case gotA:
#if TRACE
    printf("generator: state gotA\r\n");
#endif
                Aget = 1;
                Aget_off.start(SIGNAL_TIME);
                Acount++;
                state = waiting;
                break;
            
        }
        Thread::yield();
    }
}