#include "responder.h"
#include "tests.cpp"
#include "common.h"

Timer heartClock;
osEvent evt;

bool res = false;

enum responder_state{
    Random, Random_A, Random_V, Manual, Manual_A, Manual_V, Test 
};

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
                signal_times_index = 0;
                signal_times_clk.reset();
                update_keypress();
                if (keypress == 'm')
                {
                    state = Manual;  
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
                    test_0();
                    dump_signal_times();
                    break;
                }
                
                if (keypress == '1')
                {
                    clear_keypress();
                    test_1();
                    dump_signal_times();
                    break;
                }
            
                if (keypress == '2')
                {
                    clear_keypress();
                    test_2();
                    dump_signal_times();
                    break;
                }
            
                if (keypress == '3')
                {
                    clear_keypress();
                    test_3();
                    dump_signal_times();
                    break;
                }
            
                if (keypress == '4')
                {
                    clear_keypress();
                    test_4();
                    dump_signal_times();
                    break;
                }
             
                if (keypress == '5')
                {
                    clear_keypress();
                    test_5();
                    dump_signal_times();
                    break;
                }

                if (keypress == '6')
                {
                    clear_keypress();
                    test_6();
                    dump_signal_times();
                    break;
                }

                if (keypress == '7')
                {
                    clear_keypress();
                    test_7();
                    dump_signal_times();
                    break;
                }

                if (keypress == '8')
                {
                    clear_keypress();
                    test_8();
                    dump_signal_times();
                    break;
                }

                if (keypress == '9')
                {
                    clear_keypress();
                    test_9();
                    dump_signal_times();
                    break;
                }

                if (keypress == 'u')
                {
                    clear_keypress();
                    test_10();
                    dump_signal_times();
                    break;
                }

                if (keypress == 'i')
                {
                    clear_keypress();
                    test_11();
                    dump_signal_times();
                    break;
                }

                if (keypress == 'o')
                {
                    clear_keypress();
                    test_12();
                    dump_signal_times();
                    break;
                }

                if (keypress == 'p')
                {
                    clear_keypress();
                    test_13();
                    dump_signal_times();
                    break;
                }
                
        }
        Thread::yield();
    }
}
