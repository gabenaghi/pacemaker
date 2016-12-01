#include "responder.h"

Timer heartClock;
Timer testTimer;
osEvent evt;

char keypress = ' ';
bool res = false;

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
                evt = Thread::signal_wait(0x0, SIG_TIMEOUT);
                if (evt.value.signals & SIG_VPACE)
                {
                    state = Random_V;
                    break;
                }
                
                if (evt.value.signals & SIG_APACE)
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
             
                evt = Thread::signal_wait(0x0, SIG_TIMEOUT);
                if (evt.value.signals & SIG_VPACE)
                {
                    state = Manual_V;
                    break;
                }
                
                if (evt.value.signals & SIG_APACE)
                {
                    state = Manual_A;   
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
                
                if (keypress == '0')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: LRI\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if !(evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: LRI VPACE timeout\r\n");
                        break;
                    }
                
                    
                    evt = Thread::signal_wait(SIG_VPACE, TIME_LRI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI VPACE 1 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if !(evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI VPACE 1 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(19);
                    global_signal_set(SIG_VSIGNAL);


                    evt = Thread::signal_wait(SIG_VPACE, TIME_LRI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI VPACE 2 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if !(evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI VPACE 2 failed to arrive\r\n");
                        break;
                    }
                    pc.printf("Test: Passed\r\n");
                    break;  
                }
                
                if (keypress == '1')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
            
                if (keypress == '2')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
            
                if (keypress == '3')
                {
                    clear_keypress(); 
                    
                    state = Test;
                    break;  
                }
            
                if (keypress == '4')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: PVARP\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: PVARP VPACE timeout\r\n");
                    }

                    wait_ms(TIME_PVARP - 20);
                    global_signal_set(SIG_ASIGNAL);
                    Timer uri_timer;
                    uri_timer.start();
                    wait_ms(TIME_AVI + 20);
                    while (uri_timer.read_ms() <= TIME_URI);
                    global_signal_set(SIG_ASIGNAL);
                    
                    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: PVARP VPACE too early\r\n");
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: PVARP VPACE failed to arrive\r\n");
                    }

                    pc.printf("Test passed: PVARP\r\n");

                    break;  
                }
             
                if (keypress == '5')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
               
                if (keypress == '6')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
                
                if (keypress == '7')
                {
                    clear_keypress(); 
                    

                    pc.printf("Test: Passed\r\n");
                    break;  
                }
               
                if (keypress == '8')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: Normal atrium and slow ventricle (NASV)\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: NASV VPACE timeout\r\n");
                    }

                    wait_ms(TIME_URI);
                    global_signal_set(SIG_ASIGNAL);
                    
                    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: NASV VPACE 1 too early\r\n");
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: NASV VPACE 1 failed to arrive\r\n");
                    }

                    wait_ms(TIME_URI);
                    global_signal_set(SIG_ASIGNAL);

                    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: NASV VPACE 2 too early\r\n");
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: NASV VPACE 2 failed to arrive\r\n");
                    }
                    
                    pc.printf("Test passed: NASV\r\n");

                    break;  
                }
               
                if (keypress == '9')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
                
                if (keypress == 'u')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
                
                if (keypress == 'i')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
                
                if (keypress == 'o')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: slow atrium and normal ventricle (SANV)\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: SANV VPACE timeout\r\n");
                    }

                    
                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SANV APACE 1 too early\r\n");
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SANV APACE 1 failed to arrive\r\n");
                    }

                    wait_ms(TIME_AVI - 20);
                    global_signal_set(SIG_VSIGNAL);

                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SANV APACE 2 too early\r\n");
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SANV APACE 2 failed to arrive\r\n");
                    }

                    wait_ms(TIME_AVI - 20);
                    global_signal_set(SIG_VSIGNAL);

                    pc.printf("Test passed: SANV\r\n");

                    break;  
                }
                
                if (keypress == 'p')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
                    break;  
                }
                
        }
        Thread::yield();
    }
}