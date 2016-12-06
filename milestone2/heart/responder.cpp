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
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: LRI VPACE timeout\r\n");
                        break;
                    }
                
                    
                    evt = Thread::signal_wait(SIG_VPACE, TIME_LRI - TOLERANCE);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI VPACE 1 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
                    if (!(evt.value.signals & SIG_APACE))
                    {
                        pc.printf("Test: LRI VPACE 1 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(20);
                    global_signal_set(SIG_VSIGNAL);


                    evt = Thread::signal_wait(SIG_VPACE, TIME_LRI - TOLERANCE);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: LRI VPACE 2 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
                    if (!(evt.value.signals & SIG_APACE))
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

                    pc.printf("Test: VRP\r\n");

                    testTimer.reset();

                    // wait for Vpace
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: VRP VPACE timeout\r\n");
                    }
                    
                    testTimer.reset();
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
                    pc.printf("Test: AVI\r\n");
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: AVI VPACE timeout\r\n");
                        global_signal_set(SIG_VSIGNAL);
                    }
                
                    
                    evt = Thread::signal_wait(SIG_APACE, TIME_URI+20);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: AVI APACE 1 too early\r\n");
                        break;
                    }


                    evt = Thread::signal_wait(SIG_APACE, TIME_AVI);
                    if (!(evt.value.signals & SIG_APACE))
                    {
                        pc.printf("Test: AVI VPACE failed to arrive\r\n");
                        break;
                    }else{
                        pc.printf("Test: AVI test success");
                        global_signal_set(SIG_VSIGNAL);
                    }
                    
                    state = Test;
                    break;  
                }
            
                if (keypress == '3')
                {
                    clear_keypress(); 
                     
                    Timer pvarp_timer;
                    pvarp_timer.start();
                    pc.printf("Test: PVARP\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: PVARP VPACE timeout\r\n");
                        break;
                    }

                    wait_ms(TIME_PVARP - 20);
                    global_signal_set(SIG_ASIGNAL);
                    while (pvarp_timer.read_ms() <= TIME_URI + 20);
                    global_signal_set(SIG_ASIGNAL);
                    
                    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: PVARP VPACE too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: PVARP VPACE failed to arrive\r\n");
                        break;
                    }

                    pc.printf("Test passed: PVARP\r\n");
                    break;  
                }
            
                if (keypress == '4')
                {
                    clear_keypress(); 
                   
                    pc.printf("Test: URI\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: URI VPACE timeout\r\n");
                        break;
                    }

                    wait_ms(TIME_PVARP);
                    global_signal_set(SIG_ASIGNAL);

                    evt = Thread::signal_wait(SIG_VPACE, TIME_URI - TIME_PVARP - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: URI VPACE 1 too early\r\n");
                        break;
                    }
                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: URI VPACE 1 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(TIME_URI+20);
                    global_signal_set(SIG_ASIGNAL);

                    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: URI VPACE 2 too early\r\n");
                        break;
                    }
                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: URI VPACE 2 failed to arrive\r\n");
                        break;
                    }
                    pc.printf("Test passed: URI\r\n");
                    break;  
                }
             
                if (keypress == '5')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: NANV\r\n");

                    testTimer.reset();

                    // wait for Vpace
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: NANV VPACE timeout\r\n");
                    }
                    
                    testTimer.start();
                    bool failed = false;

                    // wait for TIME_URI, fail if get paced
                    while (testTimer.read_ms() < TIME_URI) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NANV fail (Vpaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NANV fail (Apaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send ASIGNAL
                    global_signal_set(SIG_ASIGNAL);
                    

                    // wait for TIME_URI + TIME_AVI - 20, fail if get paced
                    while (testTimer.read_ms() < TIME_URI + TIME_AVI - 20) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NANV fail (Vpaced before 1st Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NANV fail (Apaced before 1st Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send VSIGNAL
                    global_signal_set(SIG_VSIGNAL);

                    // wait for 2 * TIME_URI + TIME_AVI - 20, fail if get paced
                    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI - 20) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NANV fail (Vpaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NANV fail (Apaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send VSIGNAL
                    global_signal_set(SIG_VSIGNAL);

                    // wait for 2 * TIME_URI + 2 * TIME_AVI - 2 * 20, fail if get paced
                    while (testTimer.read_ms() < 2 * (TIME_URI + TIME_AVI - 20)) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NAFV fail (Vpaced before 2nd Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NANV fail (Apaced before 2nd Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    pc.printf("Test: NANV passed\r\n");

                    break;  
                }
               /*
               *Normal atrium and fast ventriculum
               *Normal A, but fast V
               */ 
                if (keypress == '6')
                {
                    clear_keypress(); 
                    pc.printf("Test: NAFV\r\n");

                    testTimer.reset();

                    // wait for Vpace
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: NAFV VPACE timeout\r\n");
                        break;
                    }
                    
                    testTimer.start();
                    bool failed = false;

                    // wait for TIME_URI, fail if get paced
                    while (testTimer.read_ms() < TIME_URI) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NAFV fail (Vpaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NAFV fail (Apaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send ASIGNAL
                    global_signal_set(SIG_ASIGNAL);
                    

                    // wait for TIME_URI + TIME_AVI - 20, fail if get paced
                    while (testTimer.read_ms() < TIME_URI + TIME_AVI/2) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NAFV fail (Vpaced before 1st Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NAFV fail (Apaced before 1st Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send VSIGNAL
                    global_signal_set(SIG_VSIGNAL);

                    // wait for 2 * TIME_URI + TIME_AVI - 20, fail if get paced
                    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI/2) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NAFV fail (Vpaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NAFV fail (Apaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send ASIGNAL
                    global_signal_set(SIG_ASIGNAL);

                    // wait for 2 * TIME_URI + 2 * TIME_AVI - 2 * 20, fail if get paced
                    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: NAFV fail (Vpaced before 2nd Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: NAFV fail (Apaced before 2nd Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                    }
                    //send VSIGNAL
                    global_signal_set(SIG_VSIGNAL);
                    if (failed) {
                        break;
                    }
                    

                    pc.printf("Test: NAFV passed\r\n");

                    break;
                    
                    state = Test;
                    break;  
                }
               
                if (keypress == '7')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: Normal atrium and slow ventricle (NASV)\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: NASV VPACE timeout\r\n");
                        break;
                    }

                    wait_ms(TIME_URI);
                    global_signal_set(SIG_ASIGNAL);
                    
                    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: NASV VPACE 1 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: NASV VPACE 1 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(TIME_URI);
                    global_signal_set(SIG_ASIGNAL);

                    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - 1);
                    if (evt.value.signals & SIG_VPACE)
                    {
                        pc.printf("Test: NASV VPACE 2 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: NASV VPACE 2 failed to arrive\r\n");
                        break;
                    }
                    
                    pc.printf("Test passed: NASV\r\n");
                    break;  
                }
               
                if (keypress == '8')
                {
                    clear_keypress(); 
                
                    pc.printf("Test: Fast atrium and normal ventriculum (FANV)\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: FANV VPACE timeout\r\n");
                        break;
                    }

                    wait_ms(TIME_PVARP + 20); 
                    global_signal_set(SIG_ASIGNAL);

                    wait_ms(TIME_AVI - 20);
                    global_signal_set(SIG_VSIGNAL);

                    wait_ms(TIME_PVARP + 20); 
                    global_signal_set(SIG_ASIGNAL);

                    wait_ms(TIME_AVI - 20);
                    global_signal_set(SIG_VSIGNAL);

                    pc.printf("Test passed: FANV\r\n");

                    break;  
                }
               
                if (keypress == '9')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: FAFV\r\n");

                    testTimer.reset();

                    // wait for Vpace
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: FAFV VPACE timeout\r\n");
                    }
                    
                    testTimer.start();
                    bool failed = false;

                    // wait for TIME_URI - 20, fail if get paced
                    while (testTimer.read_ms() < TIME_URI) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FAFV fail (Vpaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FAFV fail (Apaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send ASIGNAL
                    global_signal_set(SIG_ASIGNAL);
                    

                    // wait for TIME_URI + TIME_AVI / 2 - 20, fail if get paced
                    while (testTimer.read_ms() < TIME_URI + TIME_AVI / 2 - 20) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FAFV fail (Vpaced before 1st Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FAFV fail (Apaced before 1st Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send VSIGNAL
                    global_signal_set(SIG_VSIGNAL);

                    // wait for 2 * TIME_URI + TIME_AVI / 2 - 2 * 20, fail if get paced
                    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI / 2 - 2 * 20) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FAFV fail (Vpaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FAFV fail (Apaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send VSIGNAL
                    global_signal_set(SIG_VSIGNAL);

                    // wait for 2 * TIME_URI + TIME_AVI - 2 * 20, fail if get paced
                    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI - 2 * 20) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FAFV fail (Vpaced before 2nd Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FAFV fail (Apaced before 2nd Vsignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    pc.printf("Test: FAFV passed\r\n");

                    break;  
                }
                
                if (keypress == 'u')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: FASV\r\n");

                    testTimer.reset();

                    // wait for Vpace
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: FASV VPACE timeout\r\n");
                    }
                    
                    testTimer.start();
                    bool failed = false;

                    // wait for TIME_PVARP + 20, fail if get paced
                    while (testTimer.read_ms() < TIME_PVARP + 20) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FASV fail (Vpaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FASV fail (Apaced before 1st Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send Asignal
                    global_signal_set(SIG_ASIGNAL);

                    // wait for TIME_PVARP + 20 + TIME_AVI - 1, fail if get paced
                    while (testTimer.read_ms() < TIME_PVARP + 20 + TIME_AVI - 1) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FASV fail (Vpaced before 1st Vpace)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FASV fail (Apaced before 1st Vpace)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2ms, check if get paced correctly
                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE)) {
                        pc.printf("Test: FASV failed (didn't get 1st Vpace)\r\n");
                        failed = true;
                    }
                    else if (evt.value.signals & SIG_APACE) {
                        pc.printf("Test: FASV failed (got 1st Vpace with Apace)\r\n");
                        failed = true;
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2 * TIME_PVARP + 2 * 20 + TIME_AVI, fail if get paced
                    while (testTimer.read_ms() < TIME_PVARP + 2 * 20 + TIME_AVI) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FASV fail (Vpaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FASV fail (Apaced before 2nd Asignal)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // send Asignal
                    global_signal_set(SIG_ASIGNAL);

                    // wait for 2 * TIME_PVARP + 2 * 20 + 2 * TIME_AVI - 1, fail if get paced
                    while (testTimer.read_ms() < 2 * (TIME_PVARP + 20 + TIME_AVI) - 1) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: FASV fail (Vpaced before 2nd Vpace)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: FASV fail (Apaced before 2nd Vpace)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2ms, check if get paced correctly
                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE)) {
                        pc.printf("Test: FASV failed (didn't get 2nd Vpace)\r\n");
                        failed = true;
                    }
                    else if (evt.value.signals & SIG_APACE) {
                        pc.printf("Test: FASV failed (got 2nd Vpace with Apace)\r\n");
                        failed = true;
                    }

                    if (failed) {
                        break;
                    }
                    pc.printf("Test: FASV passed\r\n");

                    break;  
                }
                
                if (keypress == 'i')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: slow atrium and normal ventricle (SANV)\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: SANV VPACE timeout\r\n");
                        break;
                    }

                    
                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SANV APACE 1 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!(evt.value.signals & SIG_APACE))
                    {
                        pc.printf("Test: SANV APACE 1 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(TIME_AVI - 20);
                    global_signal_set(SIG_VSIGNAL);

                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SANV APACE 2 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!(evt.value.signals & SIG_APACE))
                    {
                        pc.printf("Test: SANV APACE 2 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(TIME_AVI - 20);
                    global_signal_set(SIG_VSIGNAL);

                    pc.printf("Test passed: SANV\r\n");
                    
                    break;  
                }
                
                if (keypress == 'o')
                {
                    clear_keypress(); 
                
                    pc.printf("Test: slow atrium and fast ventricle (SAFV)\r\n");
                    
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: SAFV VPACE timeout\r\n");
                        break;
                    }

                    
                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SAFV APACE 1 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!(evt.value.signals & SIG_APACE))
                    {
                        pc.printf("Test: SAFV APACE 1 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(0.5 * TIME_AVI);
                    global_signal_set(SIG_VSIGNAL);

                    evt = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - 1);
                    if (evt.value.signals & SIG_APACE)
                    {
                        pc.printf("Test: SAFV APACE 2 too early\r\n");
                        break;
                    }

                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!(evt.value.signals & SIG_APACE))
                    {
                        pc.printf("Test: SAFV APACE 2 failed to arrive\r\n");
                        break;
                    }

                    wait_ms(TIME_AVI);
                    global_signal_set(SIG_VSIGNAL);

                    pc.printf("Test passed: SAFV\r\n");

                    break;  
                }
                
                if (keypress == 'p')
                {
                    clear_keypress(); 
                    
                    pc.printf("Test: SASV\r\n");

                    testTimer.reset();

                    // wait for Vpace
                    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
                    if (!(evt.value.signals & SIG_VPACE))
                    {
                        pc.printf("Test: SASV VPACE timeout\r\n");
                    }
                    
                    testTimer.start();
                    bool failed = false;

                    // wait for TIME_LRI - TIME_AVI - 1, fail if get paced
                    while (testTimer.read_ms() < TIME_LRI - TIME_AVI - 1) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: SASV fail (Vpaced before 1st Apace)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: SASV fail (Apaced before 1st Apace)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2ms, check if get paced correctly
                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!(evt.value.signals & SIG_APACE)) {
                        pc.printf("Test: SASV failed (didn't get 1st Apace)\r\n");
                        failed = true;
                    }
                    else if (evt.value.signals & SIG_VPACE) {
                        pc.printf("Test: SASV failed (got 1st Apace with Vpace)\r\n");
                        failed = true;
                    }

                    if (failed) {
                        break;
                    }

                    // wait for TIME_LRI - 1, fail if get paced
                    while (testTimer.read_ms() < TIME_LRI - 1) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: SASV fail (Vpaced before 1st Vpace)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: SASV fail (Apaced before 1st Vpace)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2ms, check if get paced correctly
                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE)) {
                        pc.printf("Test: SASV failed (didn't get 1st Vpace)\r\n");
                        failed = true;
                    }
                    else if (evt.value.signals & SIG_APACE) {
                        pc.printf("Test: SASV failed (got 1st Vpace with Apace)\r\n");
                        failed = true;
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2 * TIME_LRI - TIME_AVI - 1, fail if get paced
                    while (testTimer.read_ms() < TIME_LRI - TIME_AVI - 1) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: SASV fail (Vpaced before 2nd Apace)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: SASV fail (Apaced before 2nd Apace)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2ms, check if get paced correctly
                    evt = Thread::signal_wait(SIG_APACE, 2);
                    if (!(evt.value.signals & SIG_APACE)) {
                        pc.printf("Test: SASV failed (didn't get 2nd Apace)\r\n");
                        failed = true;
                    }
                    else if (evt.value.signals & SIG_VPACE) {
                        pc.printf("Test: SASV failed (got 2nd Apace with Vpace)\r\n");
                        failed = true;
                    }

                    if (failed) {
                        break;
                    }

                    // wait for TIME_LRI - 1, fail if get paced
                    while (testTimer.read_ms() < 2 * TIME_LRI - 1) {
                        evt = Thread::signal_wait(0, 1);
                        if (evt.value.signals & (SIG_VPACE)) {
                          pc.printf("Test: SASV fail (Vpaced before 2nd Vpace)\r\n");
                          failed = true;
                          break;
                        }
                        if (evt.value.signals & (SIG_APACE)) {
                          pc.printf("Test: SASV fail (Apaced before 2nd Vpace)\r\n");
                          failed = true;
                          break;
                        }
                    }

                    if (failed) {
                        break;
                    }

                    // wait for 2ms, check if get paced correctly
                    evt = Thread::signal_wait(SIG_VPACE, 2);
                    if (!(evt.value.signals & SIG_VPACE)) {
                        pc.printf("Test: SASV failed (didn't get 2nd Vpace)\r\n");
                        failed = true;
                    }
                    else if (evt.value.signals & SIG_APACE) {
                        pc.printf("Test: SASV failed (got 2nd Vpace with Apace)\r\n");
                        failed = true;
                    }

                    if (failed) {
                        break;
                    }
                    pc.printf("Test: SASV passed\r\n");

                    break;  
                }


        }
        Thread::yield();
    }
}
