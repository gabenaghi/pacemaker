#include "responder.h"

/* 
input toggle implementation
bool Vpace = false;
bool Apace = false;

void resetVpace(void const * n){Vpace = false;}
void resetApace(void const * n){Apace = false;}

InterruptIn VpaceIn(VPACE_PIN);
InterruptIn ApaceIn(APACE_PIN);
RtosTimer VpaceResetTimer( resetVpace, osTimerOnce);
RtosTimer ApaceResetTimer( resetApace, osTimerOnce);
*/

DigitalIn Vpace(VPACE_PIN);
DigitalIn Apace(APACE_PIN);
Timer heartClock;
osEvent evt;
Serial pc(USBTX, USBRX);
/*
void setVpace()
{
    Vpace = true;
    VpaceResetTimer.start(ATOMIC_TIME);
}

void setApace()
{
    Apace = true;
    ApaceResetTimer.start(ATOMIC_TIME);
}
*/

enum responder_state{
    Random, Random_A, Random_V, Manual, Manual_A, Manual_V, Test 
};

void responder_thread()
{
    /*
    VpaceIn.rise(setVpace);
    VpaceIn.fall(setVpace);
    ApaceIn.rise(setApace);
    ApaceIn.fall(setApace);
    */
    
    responder_state state = Random;

    heartClock.start();
    
    while(1)
    {
        switch (state)
        {
            case Random:
                switch (rand() % 5)
                {
                    case 0:
                        if (heartClock.read_ms() >= minwait_V)
                            state = Random_V;
                        break;
                        
                    case 1:
                        if (Vpace)
                            state = Random_V;
                        break;
                    
                    case 2:
                        if (heartClock.read_ms() >= minwait_A)
                            state = Random_A;
                        break;
                    
                    case 3:   
                        if (Apace)
                            state = Random_A;
                        break;
                    
                    case 4:
                        // do nothing
                        break;
                }
                break;
            
            case Random_A:
                switch (rand() % 2)
                {
                    case 0:
                        threads[T_GENERATOR].signal_set(SIG_VSIGNAL);
                        heartClock.reset();
                    case 1: 
                        // do nothing
                        break;   
                }
                break;
            
            case Random_V:
                switch (rand() % 2)
                {
                    case 0:
                        threads[T_GENERATOR].signal_set(SIG_VSIGNAL);
                        heartClock.reset();
                    case 1: 
                        // do nothing
                        break;   
                }
                break;
                
            case Manual:
                // problem: wrong branch destroys enqueued command.
                // idea: keep scoped char for "current command", updating if available. Clear after use.
                switch (rand() % 5)
                {
                 case 0:
                    if (pc.readable())
                    {
                        if (pc.getc() == 'v')
                        {
                            state = Manual_V;   
                        }   
                    }
                    break;
                 
                 case 1:
                    if (pc.readable())
                    {
                        if (pc.getc() == 'a')
                        {
                            state = Manual_A;   
                        }   
                    }
                    break;
                 
                 case 2:
                    if (Vpace)
                    {
                        state = Manual_V;
                    }
                    break;
                    
                 case 3:
                    if (Apace)
                    {
                        state = Manual_A;   
                    }
                    break;
                    
                 case 4: 
                    //do nothing
                    break;   
                    
                }
                break;
                
            case Manual_A:
                switch (rand() % 2)
                {
                    case 0:
                        threads[T_GENERATOR].signal_set(SIG_ASIGNAL);
                        heartClock.reset();
                        break;
                    case 1:  
                        //do nothing
                        break; 
                }
                break;
            
            case Manual_V:
                switch (rand() % 2)
                {
                    case 0:
                        threads[T_GENERATOR].signal_set(SIG_VSIGNAL);
                        heartClock.reset();
                        break;
                    case 1:  
                        //do nothing
                        break; 
                }
                break;
                
            case Test:
                // ??? put tests here
                break;
            
            default:
                printf("Illegal responder state\n");
                
        }
    }
}