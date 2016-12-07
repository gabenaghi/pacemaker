
int test_5(void)
{
    pc.printf("Test: NANV\r\n");

    testTimer.reset();

    // wait for Vpace
    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt.value.signals & SIG_VPACE)) {
        pc.printf("Test: NANV VPACE timeout\r\n");
    }
                    
    testTimer.start();

    // wait for TIME_URI, fail if get paced
    while (testTimer.read_ms() < TIME_URI) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NANV fail (Vpaced before 1st Asignal)\r\n");
            return 1;
        }
        if (evt.value.signals & (SIG_APACE)) {
            pc.printf("Test: NANV fail (Apaced before 1st Asignal)\r\n");
            return 1;
        }
    }

    // send ASIGNAL
    global_signal_set(SIG_ASIGNAL);
                    

    // wait for TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < TIME_URI + TIME_AVI - 20) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NANV fail (Vpaced before 1st Vsignal)\r\n");
            return 1;
        }
        if (evt.value.signals & (SIG_APACE)) {
            pc.printf("Test: NANV fail (Apaced before 1st Vsignal)\r\n");
            return 1;
        }
    }

    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI - 20) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NANV fail (Vpaced before 2nd Asignal)\r\n");
            return 1;
        }
        if (evt.value.signals & (SIG_APACE)) {
            pc.printf("Test: NANV fail (Apaced before 2nd Asignal)\r\n");
            return 1;
        }
    }

    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + 2 * TIME_AVI - 2 * 20, fail if get paced
    while (testTimer.read_ms() < 2 * (TIME_URI + TIME_AVI - 20)) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NAFV fail (Vpaced before 2nd Vsignal)\r\n");
            return 1;
        }
        if (evt.value.signals & (SIG_APACE)) {
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
    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt.value.signals & SIG_VPACE))
    {
        pc.printf("Test: NAFV VPACE timeout\r\n");
        return 1;
    }

    testTimer.start();

    // wait for TIME_URI, fail if get paced
    while (testTimer.read_ms() < TIME_URI) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NAFV fail (Vpaced before 1st Asignal)\r\n");
            return 1;
        }
        if (evt.value.signals & (SIG_APACE)) {
            pc.printf("Test: NAFV fail (Apaced before 1st Asignal)\r\n");
            return 1;
        }
    }


    // send ASIGNAL
    global_signal_set(SIG_ASIGNAL);


    // wait for TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < TIME_URI + TIME_AVI/2) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
            pc.printf("Test: NAFV fail (Vpaced before 1st Vsignal)\r\n");
            return 1;        
        }
        if (evt.value.signals & (SIG_APACE)) {
            pc.printf("Test: NAFV fail (Apaced before 1st Vsignal)\r\n");
            return 1;
        }
    }


    // send VSIGNAL
    global_signal_set(SIG_VSIGNAL);

    // wait for 2 * TIME_URI + TIME_AVI - 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI/2) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
          pc.printf("Test: NAFV fail (Vpaced before 2nd Asignal)\r\n");
          return 1;
        }
        if (evt.value.signals & (SIG_APACE)) {
          pc.printf("Test: NAFV fail (Apaced before 2nd Asignal)\r\n");
          return 1;
        }
    }


    // send ASIGNAL
    global_signal_set(SIG_ASIGNAL);

    // wait for 2 * TIME_URI + 2 * TIME_AVI - 2 * 20, fail if get paced
    while (testTimer.read_ms() < 2 * TIME_URI + TIME_AVI) {
        evt = Thread::signal_wait(0, 1);
        if (evt.value.signals & (SIG_VPACE)) {
          pc.printf("Test: NAFV fail (Vpaced before 2nd Vsignal)\r\n");
          return 1;
        }
        if (evt.value.signals & (SIG_APACE)) {
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

    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt.value.signals & SIG_VPACE))
    {
        pc.printf("Test: NASV VPACE timeout\r\n");
        return 1;
    }

    wait_ms(TIME_URI);
    global_signal_set(SIG_ASIGNAL);

    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - TOLERANCE);
    if (evt.value.signals & SIG_VPACE)
    {
        pc.printf("Test: NASV VPACE 1 too early\r\n");
        return 1;
    }

    evt = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt.value.signals & SIG_VPACE))
    {
        pc.printf("Test: NASV VPACE 1 failed to arrive\r\n");
        return 1;
    }

    wait_ms(TIME_URI);
    global_signal_set(SIG_ASIGNAL);

    evt = Thread::signal_wait(SIG_VPACE, TIME_AVI - TOLERANCE);
    if (evt.value.signals & SIG_VPACE)
    {
        pc.printf("Test: NASV VPACE 2 too early\r\n");
        return 1;
    }

    evt = Thread::signal_wait(SIG_VPACE, TWO_TOLERANCE);
    if (!(evt.value.signals & SIG_VPACE))
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

    evt = Thread::signal_wait(SIG_VPACE, TEST_START_TIMEOUT);
    if (!(evt.value.signals & SIG_VPACE))
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

