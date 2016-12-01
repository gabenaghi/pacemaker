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
                        state = Test;
                    }
                
                    
                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI APACE 1 too early\r\n");
                        state = Test;
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if !(evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI APACE 1 failed to arrive\r\n");
                        state = Test;
                    }

                    wait_ms(19);
                    global_signal_set(SIG_VSIGNAL);


                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI APACE 2 too early\r\n");
                        state = Test;
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if !(evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI APACE 2 failed to arrive\r\n");
                        state = Test;
                    }
                    
                    state = Test;
                    break;  
                }
                
                if (keypress == '1')
                {
                    clear_keypress(); 

                    pc.printf("Test: VRP\r\n");

                    // wait for Vpace
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if !(evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: VRP VPACE timeout\r\n");
                    }
                    
                    testTimer.start();
                    bool failed = false;
                    
                    // wait for TIME_VRP - 20, fail if get paced
                    while (testTimer.read_ms() < TIME_VRP - 20) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE | SIG_APACE)) {
                          pc.printf("Test: VRP fail (paced before Vsense)\r\n");
                          failed = true;
                        }
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send VSIGNAL
                    global_signal_set(SIG_VSIGNAL);

                    bool Apaced = false;
                    // wait for TIME_LRI +/- 1, fail if get Vpace or Apace twice 
                    while (testTimer.read_ms() < TIME_LRI - 1) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & SIG_VPACE) {
                            pc.printf("Test: VRP fail (Vpaced too early)\r\n");
                            failed = true;
                            break;
                        }
                        if (evt.value.signals & SIG_APACE) {
                            if (Apaced) {
                                pc.printf("Test: VRP failed (Apaced twice before Vpace)\r\n");
                                failed = true;
                                break;
                            }
                            else {
                                Apaced = true;
                            }
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2ms
                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE)) {
                        pc.printf("Test: VRP failed (didn't get Vpace at TIME_LRI)\r\n");
                        failed = true;
                    }
                    else if (evt.value.signals & SIG_APACE) {
                        pc.printf("Test: VRP failed (got Apace and Vpace at TIME_LRI)\r\n");
                        failed = true;
                    }

                    pc.printf("Test: VRP passed\r\n");
                    
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
                    
                    
                    state = Test;
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
                    
                    
                    state = Test;
                    break;  
                }
               
                if (keypress == '8')
                {
                    clear_keypress(); 
                    
                    
                    state = Test;
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
                    
                    
                    state = Test;
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
