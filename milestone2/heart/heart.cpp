#include "common.h"
#include "generator.h"
#include "responder.h"

Thread threads[NUM_THREADS] = {Thread(osPriorityNormal)};

DigitalIn Vpace(VPACE_PIN);
DigitalIn Apace(APACE_PIN);

DigitalOut LED[4] = {DigitalOut(LED1),DigitalOut(LED2),DigitalOut(LED3),DigitalOut(LED4)};

int main()
{    
    threads[T_GENERATOR].start(generator_thread);
    threads[T_RESPONDER].start(responder_thread);
    while(1)
    {
        Thread::yield();
    }
    return 0; 
}