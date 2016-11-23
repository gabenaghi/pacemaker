#include "responder.h"


bool Vpace = false;
bool Apace = false;

void resetVpace(void const * n){Vpace = false;}
void resetApace(void const * n){Apace = false;}

InterruptIn VpaceIn(VPACE_PIN);
InterruptIn ApaceIn(APACE_PIN);
RtosTimer VpaceResetTimer( resetVpace, osTimerOnce);
RtosTimer ApaceResetTimer( resetApace, osTimerOnce);

Timer heartClock;

osEvent evt;

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


enum responder_state{
    Random, Random_A, Random_V, Manual, Manual_A, Manual_V, Test 
};

void responder_thread()
{
    VpaceIn.rise(setVpace);
    VpaceIn.fall(setVpace);
    ApaceIn.rise(setApace);
    ApaceIn.fall(setApace);
    
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
            
            
                break;
            
            case Random_V:
            
                break;
                
            case Manual:
            
                break;
                
            case Manual_A:
            
                break;
            
            case Manual_V:
            
                break;
                
            case Test:
            
                break;
            
            default:
                printf("Illegal responder state\n");
                
        }
    }
}