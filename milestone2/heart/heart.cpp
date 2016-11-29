#include "common.h"
#include "generator.h"
#include "responder.h"
#include "display.h"

Thread threads[NUM_THREADS] = {Thread(osPriorityNormal)};

DigitalIn Vpace(VPACE_PIN);
DigitalIn Apace(APACE_PIN);

DigitalOut LED[4] = {DigitalOut(LED1),DigitalOut(LED2),DigitalOut(LED3),DigitalOut(LED4)};

int Acount = 0;
int Vcount = 0;

int main()
{    
    threads[T_GENERATOR].start(generator_thread);
    threads[T_RESPONDER].start(responder_thread);
<<<<<<< HEAD
    threads[T_LED].start(led_thread);
    threads[T_EXTERNAL].start(external_signals_thread);
=======
    threads[T_DISPLAY].start(display_thread);
>>>>>>> 14b199becf1d7881c0f1c705bac30c3fea2a8f68
    while(1)
    {
        Thread::signal_wait(NULL); //sleep forever
        printf("problem: main woke up\n");
    }
    return 0; 
}