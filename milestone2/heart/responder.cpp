#include "responder.h"

DigitalIn Vpace(VPACE_PIN);
DigitalIn Apace(APACE_PIN);
Timer heartClock;
osEvent evt;
Serial pc(USBTX, USBRX);

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
}

void clear_keypress()
{   
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
                switch (rand() % 7)
                {
                 case 0:
                    update_keypress();
                    if (keypress == 'v')
                    {
                        state = Manual_V; 
                        clear_keypress();   
                    }
                    break;
                 
                 case 1:
                    update_keypress();
                     if (keypress == 'a')
                    {
                        state = Manual_A;   
                        clear_keypress();
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
                    update_keypress();
                     if (keypress == 't')
                    {
                        state = Test;   
                        clear_keypress();
                    }   
                    break;
                 
                 case 5:
                    update_keypress();
                     if (keypress == 'r')
                    {
                        state = Random;   
                        clear_keypress();
                    }   
                    break;
                 
                 case 6:
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
    }
}