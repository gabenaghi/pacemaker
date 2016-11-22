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

Thread* threads[NUM_THREADS];

int main() {

	seed_lfsr();

	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i] = new Thread();
	}

	threads[T_LRI]->start(lri_thread);
    threads[T_AVI]->start(avi_thread);
    threads[T_URI]->start(uri_thread);
    threads[T_PVARP]->start(pvarp_thread);
    threads[T_VRP]->start(vrp_thread);
    threads[T_KEYBOARD]->start(keyboard_thread);
    threads[T_HEART_RATE_DISPLAY]->start(heart_rate_display_thread);
    threads[T_EXTERNAL_SIGNALS]->start(external_signals_thread);

    for (int i = 0; i < NUM_THREADS; i++) {
    	delete(threads[i]);
    }
	return 0;
}