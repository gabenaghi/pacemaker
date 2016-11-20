#include "responder.h"

enum responder_state{
    Random, Random_A, Random_V, Manual, Manual_A, Manual_V, Test 
};

void responder_thread()
{
    responder_state state = Random;
    osEvent evt;
    Timer heartClock;
    
    while(1)
    {
        switch (state)
        {
            case Random:
                
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