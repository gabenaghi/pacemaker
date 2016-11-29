#include "common.h"
#include "generator.h"
#include "responder.h"
#include "external_signals.h"
#include "led.h"
#include "display.h"

Thread threads[NUM_THREADS] = {Thread(osPriorityNormal)};





int Acount = 0;
int Vcount = 0;

int main()
{    
    threads[T_GENERATOR].start(generator_thread);
    threads[T_RESPONDER].start(responder_thread);
    threads[T_LED].start(led_thread);
    threads[T_EXTERNAL].start(external_signals_thread);
    threads[T_DISPLAY].start(display_thread);
#if TRACE
    printf("done spawning threads\r\n");
#endif
    while(1)
    {
#if TRACE
printf("main\r\n");
#endif
        Thread::yield();
    }
    return 0; 
}