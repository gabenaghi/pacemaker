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
    
#if TRACE
printf("responder\r\n");
#endif
    
    while(1)
    {
        switch (state)
        {
            case Random:
                switch (rand() % 7)
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
                        update_keypress();
                        if (keypress == 't')
                        {
                            state = Test;
                            clear_keypress();   
                        }
                        break;
                        
                    case 5:
                        update_keypress();
                        if (keypress == 'm')
                        {
                            state = Manual;
                            clear_keypress();   
                        }
                        break;
                    
                    case 6:
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
                        //threads[T_GENERATOR].signal_set(SIG_VSIGNAL);
                        global_signal_set(SIG_VSIGNAL);
                        heartClock.reset();
                    case 1: 
                        // do nothing
                        break;   
                }
                break;
                
            case Manual:
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
                
            case Manual_A:
                switch (rand() % 2)
                {
                    case 0:
                        //threads[T_GENERATOR].signal_set(SIG_ASIGNAL);
                        global_signal_set(SIG_VSIGNAL);
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
                        //threads[T_GENERATOR].signal_set(SIG_VSIGNAL);
                        global_signal_set(SIG_VSIGNAL);
                        heartClock.reset();
                        break;
                    case 1:  
                        //do nothing
                        break; 
                }
                break;
                
            case Test:
                // put tests here
                switch (rand() % 3)
                {
                    case 0:
                        update_keypress();
                        if (keypress == 'm')
                        {
                            state = Manual;
                            clear_keypress();   
                        }
                        break;
                        
                    case 1:  
                        update_keypress();
                        if (keypress == 'r')
                        {
                            state = Random;
                            clear_keypress();   
                        }
                        break;
                    
                    case 2:
                        //do nothing
                        break; 
                }
                break;
        }
        Thread::yield();
    }
}