#include "tests.h"
#include "common.h"

osEvent evt2;
Timer testTimer;

int test_0(void)
{
    clear_keypress(); 
    pc.printf("Test: LRI\r\n");

    testTimer.stop();
    testTimer.reset();

    clear_own_signals(T_RESPONDER);
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: LRI VPACE timeout\r\n");
        return 1;
    }

    testTimer.start();

    while (testTimer.read_ms() < TIME_LRI - TOLERANCE) {
        clear_own_signals(T_RESPONDER);
        evt2 = Thread::signal_wait(SIG_VPACE, 1);
        if (evt2.value.signals & SIG_VPACE)
        {
            pc.printf("Test: LRI VPACE 1 too early\r\n");
            return 1;
        }
    }
    
    clear_own_signals(T_RESPONDER);
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: LRI VPACE 1 failed to arrive\r\n");
        return 1;
    }

    while (testTimer.read_ms() < TIME_LRI + TIME_VRP + 20);
    global_signal_set(SIG_VSIGNAL);

    while (testTimer.read_ms() < 2 * TIME_LRI + 20 - TOLERANCE) {
        clear_own_signals(T_RESPONDER);
        evt2 = Thread::signal_wait(SIG_VPACE, 1);
        if (evt2.value.signals & SIG_VPACE)
        {
            pc.printf("Test: LRI VPACE 2 too early\r\n");
            return 1;
        }
    }

    clear_own_signals(T_RESPONDER);
    evt2 = Thread::signal_wait(SIG_VPACE, TIME_VRP + TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: LRI VPACE 2 failed to arrive\r\n");
        return 1;
    }
    pc.printf("Test: Passed\r\n");
    return 0;  
}

int test_1(void)
{
    clear_keypress(); 

    pc.printf("Test: VRP\r\n");

    testTimer.stop();
    testTimer.reset();

    // wait for Vpace
    clear_own_signals(T_RESPONDER);
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: VRP VPACE timeout\r\n");
        return 1;
    }

    testTimer.start();
    
    clear_own_signals(T_RESPONDER);
    // wait for TIME_VRP - 20, fail if get paced
    while (testTimer.read_ms() < TIME_VRP - 20) {
        evt2 = Thread::signal_wait(0, 1);
        //if (evt2.value.signals & (SIG_APACE)) {
        //  pc.printf("Test: VRP fail (Apaced before Vsense)\r\n");
        //  failed = true;
        //  break;
        //}
        if (evt2.value.signals & SIG_VPACE) {
          pc.printf("Test: VRP fail (Vpaced before Vsense)\r\n");
          return 1;
        } 
    }

    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    bool Apaced = false;
    // wait for TIME_LRI +/- 1, fail if get Vpace or Apace twice 
    while (testTimer.read_ms() < TIME_LRI - TOLERANCE) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & SIG_VPACE) {
            pc.printf("Test: VRP fail (Vpaced too early)\r\n");
            return 1;
        }
        if (evt2.value.signals & SIG_APACE) {
            if (Apaced) {
                pc.printf("Test: VRP failed (Apaced twice before Vpace)\r\n");
                return 1;
            }
            else {
                Apaced = true;
            }
        }
    }

    // wait for 2 * tolerance ms
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE)) {
        pc.printf("Test: VRP failed (didn't get Vpace at TIME_LRI)\r\n");
        return 1;
    }
    else if (evt2.value.signals & SIG_APACE) {
        pc.printf("Test: VRP failed (got Apace and Vpace at TIME_LRI)\r\n");
        return 1;
    }

    pc.printf("Test: VRP passed\r\n");
    
    return 0;  
}

int test_2(void) 
{
    clear_keypress(); 
    pc.printf("Test: AVI\r\n");
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: AVI VPACE timeout\r\n");
        global_signal_set(SIG_VSIGNAL);
        return 1;
    }

    
    evt2 = Thread::signal_wait(SIG_APACE, TIME_URI+20);
    if (evt2.value.signals & SIG_APACE)
    {
        pc.printf("Test: AVI APACE 1 too early\r\n");
        return 1;
    }


    evt2 = Thread::signal_wait(SIG_APACE, TIME_AVI);
    if (!(evt2.value.signals & SIG_APACE))
    {
        pc.printf("Test: AVI VPACE failed to arrive\r\n");
        return 1;
    } else {
        pc.printf("Test: AVI test success");
        global_signal_set(SIG_VSIGNAL);
    }
    
    return 0;  
}

int test_3(void)
{
    clear_keypress(); 
     
    testTimer.stop();
    testTimer.reset();
    pc.printf("Test: PVARP\r\n");
    
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: PVARP VPACE timeout\r\n");
        return 1;
    }

    wait_ms(TIME_PVARP - 20);
    global_signal_set(SIG_ASIGNAL);
    while (testTimer.read_ms() <= TIME_URI + 20);
    global_signal_set(SIG_ASIGNAL);
    
    evt2 = Thread::signal_wait(SIG_VPACE, TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_VPACE)
    {
        pc.printf("Test: PVARP VPACE too early\r\n");
        return 1;
    }

    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: PVARP VPACE failed to arrive\r\n");
        return 1;
    }

    pc.printf("Test passed: PVARP\r\n");
    return 0;  
}

int test_4(void)
{
    clear_keypress(); 
   
    pc.printf("Test: URI\r\n");
    
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: URI VPACE timeout\r\n");
        return 1;
    }

    wait_ms(TIME_PVARP);
    global_signal_set(SIG_ASIGNAL);

    evt2 = Thread::signal_wait(SIG_VPACE, TIME_URI - TIME_PVARP - TOLERANCE);
    if (evt2.value.signals & SIG_VPACE)
    {
        pc.printf("Test: URI VPACE 1 too early\r\n");
        return 1;
    }
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: URI VPACE 1 failed to arrive\r\n");
        return 1;
    }

    wait_ms(TIME_URI+20);
    global_signal_set(SIG_ASIGNAL);

    evt2 = Thread::signal_wait(SIG_VPACE, TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_VPACE)
    {
        pc.printf("Test: URI VPACE 2 too early\r\n");
        return 1;
    }
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: URI VPACE 2 failed to arrive\r\n");
        return 1;
    }
    pc.printf("Test passed: URI\r\n");
    return 0;  
}


int test_5(void)
{
    pc.printf("Test: NANV\r\n");

    testTimer.reset();

    // wait for Vpace
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE)) {
        pc.printf("Test: NANV VPACE timeout\r\n");
    }
                    
    testTimer.start();

    // wait for TIME_URI, fail if get paced
    while (testTimer.read_ms() < TIME_URI) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NANV fail (Vpaced before 1st Asignal)\r\n");
            return 1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
            pc.printf("Test: NANV fail (Apaced before 1st Asignal)\r\n");
            return 1;
        }
    }

    // send ASIGNAL
    global_signal_set(SIG_ASIGNAL);
                    

    // wait for TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < TIME_URI + TIME_AVI - 20) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NANV fail (Vpaced before 1st Vsignal)\r\n");
            return 1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
            pc.printf("Test: NANV fail (Apaced before 1st Vsignal)\r\n");
            return 1;
        }
    }

    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI - 20) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NANV fail (Vpaced before 2nd Asignal)\r\n");
            return 1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
            pc.printf("Test: NANV fail (Apaced before 2nd Asignal)\r\n");
            return 1;
        }
    }

    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + 2 * TIME_AVI - 2 * 20, fail if get paced
    while (testTimer.read_ms() < 2 * (TIME_URI + TIME_AVI - 20)) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NAFV fail (Vpaced before 2nd Vsignal)\r\n");
            return 1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
            pc.printf("Test: NANV fail (Apaced before 2nd Vsignal)\r\n");
            return 1;
        }
    }

    pc.printf("Test: NANV passed\r\n");
    return 0;
}

int test_6(void)
{
    pc.printf("Test: NAFV\r\n");

    testTimer.reset();

    // wait for Vpace
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: NAFV VPACE timeout\r\n");
        return 1;
    }

    testTimer.start();

    // wait for TIME_URI, fail if get paced
    while (testTimer.read_ms() < TIME_URI) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NAFV fail (Vpaced before 1st Asignal)\r\n");
            return 1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
            pc.printf("Test: NAFV fail (Apaced before 1st Asignal)\r\n");
            return 1;
        }
    }


    // send ASIGNAL
    global_signal_set(SIG_ASIGNAL);


    // wait for TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < TIME_URI + TIME_AVI/2) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NAFV fail (Vpaced before 1st Vsignal)\r\n");
            return 1;        
        }
        if (evt2.value.signals & (SIG_APACE)) {
            pc.printf("Test: NAFV fail (Apaced before 1st Vsignal)\r\n");
            return 1;
        }
    }


    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI/2) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: NAFV fail (Vpaced before 2nd Asignal)\r\n");
          return 1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: NAFV fail (Apaced before 2nd Asignal)\r\n");
          return 1;
        }
    }


    // send ASIGNAL
    global_signal_set(SIG_ASIGNAL);

    // wait for 2 * TIME_URI + 2 * TIME_AVI - 2 * 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: NAFV fail (Vpaced before 2nd Vsignal)\r\n");
          return 1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: NAFV fail (Apaced before 2nd Vsignal)\r\n");
          return 1;
        }
    }
    //send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    pc.printf("Test: NAFV passed\r\n");
    return 0;
}

int test_7(void)
{
pc.printf("Test: Normal atrium and slow ventricle (NASV)\r\n");

    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: NASV VPACE timeout\r\n");
        return 1;
    }

    wait_ms(TIME_URI);
    global_signal_set(SIG_ASIGNAL);

    evt2 = Thread::signal_wait(SIG_VPACE, TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_VPACE)
    {
        pc.printf("Test: NASV VPACE 1 too early\r\n");
        return 1;
    }

    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: NASV VPACE 1 failed to arrive\r\n");
        return 1;
    }

    wait_ms(TIME_URI);
    global_signal_set(SIG_ASIGNAL);

    evt2 = Thread::signal_wait(SIG_VPACE, TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_VPACE)
    {
        pc.printf("Test: NASV VPACE 2 too early\r\n");
        return 1;
    }

    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: NASV VPACE 2 failed to arrive\r\n");
        return 1;
    }

    pc.printf("Test passed: NASV\r\n");
    return 0;
}

int test_8(void)
{
pc.printf("Test: Fast atrium and normal ventriculum (FANV)\r\n");

    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: FANV VPACE timeout\r\n");
        return 1;
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
    return 0;
}

int test_9(void)
{
      pc.printf("Test: FAFV\r\n");

    testTimer.reset();

    // wait for Vpace
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: FAFV VPACE timeout\r\n");
        return -1;
    }
    
    testTimer.start();

    // wait for TIME_URI - 20, fail if get paced
    while (testTimer.read_ms() < TIME_URI) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FAFV fail (Vpaced before 1st Asignal)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FAFV fail (Apaced before 1st Asignal)\r\n");
          return -1;
        }
    }

    // send ASIGNAL
    global_signal_set(SIG_ASIGNAL);
    

    // wait for TIME_URI + TIME_AVI / 2 - 20, fail if get paced
    while (testTimer.read_ms() < TIME_URI + TIME_AVI / 2 - 20) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FAFV fail (Vpaced before 1st Vsignal)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FAFV fail (Apaced before 1st Vsignal)\r\n");
          return -1;
        }
    }

    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + TIME_AVI / 2 - 2 * 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI / 2 - 2 * 20) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FAFV fail (Vpaced before 2nd Asignal)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FAFV fail (Apaced before 2nd Asignal)\r\n");
          return -1;
        }
    }

    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + TIME_AVI - 2 * 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI - 2 * 20) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FAFV fail (Vpaced before 2nd Vsignal)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FAFV fail (Apaced before 2nd Vsignal)\r\n");
          return -1;
        }
    }

    pc.printf("Test: FAFV passed\r\n");
    return 0;
}

int test_10(void)
{                   
    pc.printf("Test: FASV\r\n");

    testTimer.reset();

    // wait for Vpace
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: FASV VPACE timeout\r\n");
        return -1;
    }
    
    testTimer.start();

    // wait for TIME_PVARP + 20, fail if get paced
    while (testTimer.read_ms() < TIME_PVARP + 20) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FASV fail (Vpaced before 1st Asignal)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FASV fail (Apaced before 1st Asignal)\r\n");
          return -1;
        }
    }

    // send Asignal
    global_signal_set(SIG_ASIGNAL);

    // wait for TIME_PVARP + 20 + TIME_AVI - 1, fail if get paced
    while (testTimer.read_ms() < TIME_PVARP + 20 + TIME_AVI - TOLERANCE) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FASV fail (Vpaced before 1st Vpace)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FASV fail (Apaced before 1st Vpace)\r\n");
          return -1;
        }
    }

    // wait for 2ms, check if get paced correctly
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE)) {
        pc.printf("Test: FASV failed (didn't get 1st Vpace)\r\n");
        return -1;
    }
    else if (evt2.value.signals & SIG_APACE) {
        pc.printf("Test: FASV failed (got 1st Vpace with Apace)\r\n");
        return -1;
    }

    // wait for 2 * TIME_PVARP + 2 * 20 + TIME_AVI, fail if get paced
    while (testTimer.read_ms() < TIME_PVARP + 2 * 20 + TIME_AVI) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FASV fail (Vpaced before 2nd Asignal)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FASV fail (Apaced before 2nd Asignal)\r\n");
          return -1;
        }
    }

    // send Asignal
    global_signal_set(SIG_ASIGNAL);

    // wait for 2 * TIME_PVARP + 2 * 20 + 2 * TIME_AVI - 1, fail if get paced
    while (testTimer.read_ms() < 2 * (TIME_PVARP + 20 + TIME_AVI) - TOLERANCE) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: FASV fail (Vpaced before 2nd Vpace)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: FASV fail (Apaced before 2nd Vpace)\r\n");
          return -1;
        }
    }

    // wait for 2ms, check if get paced correctly
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE)) {
        pc.printf("Test: FASV failed (didn't get 2nd Vpace)\r\n");
        return -1;
    }
    else if (evt2.value.signals & SIG_APACE) {
        pc.printf("Test: FASV failed (got 2nd Vpace with Apace)\r\n");
        return -1;
    }

    pc.printf("Test: FASV passed\r\n");
    return 0;
}

int test_11(void)
{
    pc.printf("Test: slow atrium and normal ventricle (SANV)\r\n");
                    
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: SANV VPACE timeout\r\n");
        return -1;
    }

    
    evt2 = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_APACE)
    {
        pc.printf("Test: SANV APACE 1 too early\r\n");
        return -1;
    }

    evt2 = Thread::signal_wait(SIG_APACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_APACE))
    {
        pc.printf("Test: SANV APACE 1 failed to arrive\r\n");
        return -1;
    }

    wait_ms(TIME_AVI - 20);
    global_signal_set(SIG_VSIGNAL);

    evt2 = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_APACE)
    {
        pc.printf("Test: SANV APACE 2 too early\r\n");
        return -1;
    }

    evt2 = Thread::signal_wait(SIG_APACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_APACE))
    {
        pc.printf("Test: SANV APACE 2 failed to arrive\r\n");
        return -1;
    }

    wait_ms(TIME_AVI - 20);
    global_signal_set(SIG_VSIGNAL);

    pc.printf("Test passed: SANV\r\n");
    
    return 0;   
}

int test_12(void)
{
    pc.printf("Test: slow atrium and fast ventricle (SAFV)\r\n");
                    
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: SAFV VPACE timeout\r\n");
        return -1;
    }

    
    evt2 = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_APACE)
    {
        pc.printf("Test: SAFV APACE 1 too early\r\n");
        return -1;
    }

    evt2 = Thread::signal_wait(SIG_APACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_APACE))
    {
        pc.printf("Test: SAFV APACE 1 failed to arrive\r\n");
        return -1;
    }

    wait_ms(0.5 * TIME_AVI);
    global_signal_set(SIG_VSIGNAL);

    evt2 = Thread::signal_wait(SIG_APACE, TIME_LRI - TIME_AVI - TOLERANCE);
    if (evt2.value.signals & SIG_APACE)
    {
        pc.printf("Test: SAFV APACE 2 too early\r\n");
        return -1;
    }

    evt2 = Thread::signal_wait(SIG_APACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_APACE))
    {
        pc.printf("Test: SAFV APACE 2 failed to arrive\r\n");
        return -1;
    }

    wait_ms(TIME_AVI);
    global_signal_set(SIG_VSIGNAL);

    pc.printf("Test passed: SAFV\r\n");

    return 0;   
}

int test_13(void)
{
    pc.printf("Test: SASV\r\n");

    testTimer.reset();

    // wait for Vpace
    evt2 = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt2.value.signals & SIG_VPACE))
    {
        pc.printf("Test: SASV VPACE timeout\r\n");
        return -1;
    }
    
    testTimer.start();

    // wait for TIME_LRI - TIME_AVI - 1, fail if get paced
    while (testTimer.read_ms() < TIME_LRI - TIME_AVI - TOLERANCE) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: SASV fail (Vpaced before 1st Apace)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: SASV fail (Apaced before 1st Apace)\r\n");
          return -1;
        }
    }

    // wait for 2ms, check if get paced correctly
    evt2 = Thread::signal_wait(SIG_APACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_APACE)) {
        pc.printf("Test: SASV failed (didn't get 1st Apace)\r\n");
        return -1;
    }
    else if (evt2.value.signals & SIG_VPACE) {
        pc.printf("Test: SASV failed (got 1st Apace with Vpace)\r\n");
        return -1;
    }

    // wait for TIME_LRI - 1, fail if get paced
    while (testTimer.read_ms() < TIME_LRI - TOLERANCE) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: SASV fail (Vpaced before 1st Vpace)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: SASV fail (Apaced before 1st Vpace)\r\n");
          return -1;
        }
    }

    // wait for 2ms, check if get paced correctly
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE)) {
        pc.printf("Test: SASV failed (didn't get 1st Vpace)\r\n");
        return -1;
    }
    else if (evt2.value.signals & SIG_APACE) {
        pc.printf("Test: SASV failed (got 1st Vpace with Apace)\r\n");
        return -1;
    }

    // wait for 2 * TIME_LRI - TIME_AVI - 1, fail if get paced
    while (testTimer.read_ms() < TIME_LRI - TIME_AVI - TOLERANCE) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: SASV fail (Vpaced before 2nd Apace)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: SASV fail (Apaced before 2nd Apace)\r\n");
          return -1;
        }
    }

    // wait for 2ms, check if get paced correctly
    evt2 = Thread::signal_wait(SIG_APACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_APACE)) {
        pc.printf("Test: SASV failed (didn't get 2nd Apace)\r\n");
        return -1;
    }
    else if (evt2.value.signals & SIG_VPACE) {
        pc.printf("Test: SASV failed (got 2nd Apace with Vpace)\r\n");
        return -1;
    }

    // wait for TIME_LRI - 1, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_LRI - TOLERANCE) {
        evt2 = Thread::signal_wait(0, 1);
        if (evt2.value.signals & (SIG_VPACE)) {
          pc.printf("Test: SASV fail (Vpaced before 2nd Vpace)\r\n");
          return -1;
        }
        if (evt2.value.signals & (SIG_APACE)) {
          pc.printf("Test: SASV fail (Apaced before 2nd Vpace)\r\n");
          return -1;
        }
    }

    // wait for 2ms, check if get paced correctly
    evt2 = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt2.value.signals & SIG_VPACE)) {
        pc.printf("Test: SASV failed (didn't get 2nd Vpace)\r\n");
        return -1;
    }
    else if (evt2.value.signals & SIG_APACE) {
        pc.printf("Test: SASV failed (got 2nd Vpace with Apace)\r\n");
        return -1;
    }

    pc.printf("Test: SASV passed\r\n");

    return 0;  
}
