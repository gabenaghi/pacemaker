#include "responder.h"

Timer heartClock;
osEvent evt;

char keypress = ' ';

enum responder_state{
    Random, Random_A, Random_V, Manual, Manual_A, Manual_V, Test 
};

void update_keypress()
{
    while (pc.readable())
    {
        keypress = pc.getc();
    }   
    
#if TRACE
printf("updated keypress to '%c'\r\n", keypress);
#endif
}

void clear_keypress()
{   
#if TRACE
printf("clearing keypress '%c'\r\n", keypress);
#endif
    keypress = ' ';   
}

void responder_thread()
{ 
    responder_state state = Random;

    heartClock.start();
    
    while(1)
    {
        switch (state)
        {
            case Random:
#if TRACE
printf("responder: state Random\r\n");
#endif
                update_keypress();
                if (keypress == 't')
                {
                    state = Test;
                    clear_keypress(); 
                    break;  
                }
                
                if (keypress == 'm')
                {
                    state = Manual;
                    clear_keypress();   
                    break;
                }                
                if (Vpace)
                {
                    state = Random_V;
                    break;
                }
                if (Apace)
                {
                    state = Random_A;
                    break;
                }
                
                switch (rand() % 3)
                {
                    case 0:
                        if (heartClock.read_ms() >= minwait_V)
                            state = Random_V;
                        break;
                    
                    case 2:
                        if (heartClock.read_ms() >= minwait_A)
                            state = Random_A;
                        break;
                    
                    case 3:   
                        // do nothing (randomness)
                        break;
                }
                break;
            
            case Random_A:
#if TRACE
printf("responder: state RandomA\r\n");
#endif
                global_signal_set(SIG_ASIGNAL);
                heartClock.reset();
                state = Random;
                break;
            
            case Random_V:
#if TRACE
printf("responder: state RandomV\r\n");
#endif
                global_signal_set(SIG_VSIGNAL);
                heartClock.reset();
                state = Random;
                break;
                
            case Manual:
#if TRACE
printf("responder: state Manual\r\n");
#endif
                update_keypress();
                if (keypress == 'v')
                {
                    state = Manual_V; 
                    clear_keypress(); 
                    break;  
                }
            
                 if (keypress == 'a')
                {
                    state = Manual_A;   
                    clear_keypress();
                    break;
                }   
             
                if (Vpace)
                {
                    state = Manual_V;
                    break;
                }
                
                if (Apace)
                {
                    state = Manual_A;   
                    break;
                }
                
                if (keypress == 't')
                {
                    state = Test;   
                    clear_keypress();
                    break;
                }   
             
                if (keypress == 'r')
                {
                    state = Random;   
                    clear_keypress();
                    break;
                }   
                break;
                
            case Manual_A:

                global_signal_set(SIG_ASIGNAL);
                state = Manual;
                break;
            
            case Manual_V:
            
                global_signal_set(SIG_VSIGNAL);
                state = Manual;
                break;

                
            case Test:
#if TRACE
printf("responder: state Test\r\n");
#endif      
                update_keypress();
                if (keypress == 'm')
                {
                    state = Manual;
                    clear_keypress();   
                    break;
                }
                
                if (keypress == 'r')
                {
                    state = Random;
                    clear_keypress(); 
                    break;  
                }
            
                // put tests here
        }
        Thread::yield();
    }
}