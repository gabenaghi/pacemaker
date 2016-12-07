#include <stdlib.h>
#include "common.h"
#include "lri.h"
#include "avi.h"
#include "uri.h"
#include "pvarp.h"
#include "vrp.h"
#include "keyboard.h"
#include "heart_rate_display.h"
#include "external_signals.h"

#define NEW_STACK_SIZE 1024

Thread threads[NUM_THREADS] = {Thread(osPriorityNormal, NEW_STACK_SIZE, NULL)};

int main() {

	seed_lfsr();
    clk.start();
    speaker.write(0.0f);
    speaker.period_ms(100);

    //safe_println("%d", DEFAULT_STACK_SIZE);

	safe_println("Starting threads...");

	threads[T_LRI].start(lri_thread);
    threads[T_AVI].start(avi_thread);
    threads[T_URI].start(uri_thread);
    threads[T_PVARP].start(pvarp_thread);
    threads[T_VRP].start(vrp_thread);
    threads[T_KEYBOARD].start(keyboard_thread);
    //safe_println("6");
    threads[T_HEART_RATE_DISPLAY].start(heart_rate_display_thread);
    //safe_println("7");
    threads[T_EXTERNAL_SIGNALS].start(external_signals_thread);

    safe_println("All threads started successfully!");

    Thread::wait(osWaitForever);

	return 0;
}

