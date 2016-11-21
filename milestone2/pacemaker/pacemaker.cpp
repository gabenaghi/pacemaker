#include "common.h"
#include "lri.h"
#include "avi.h"
#include "uri.h"
#include "pvarp.h"
#include "vrp.h"
#include "keyboard.h"
#include "heart_rate_display.h"

Thread threads[NUM_THREADS];

int main() {
	threads[T_LRI].start(generator_thread);
    threads[T_AVI].start(responder_thread);
    threads[T_URI].start(generator_thread);
    threads[T_PVARP].start(responder_thread);
    threads[T_VRP].start(generator_thread);
    threads[T_KEYBOARD].start(responder_thread);
    threads[T_HEART_RATE_DISPLAY].start(generator_thread);
	return 0;
}