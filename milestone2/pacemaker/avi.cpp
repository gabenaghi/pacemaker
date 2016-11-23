// avi.cpp
#include "common.h"
#include "avi.h"

enum avi_state {
	idle,
	avi,
	wait_uri,
}

void clear_own_signals()
{
	threads[T_AVI]->signal_clr(SIG_ASENSE);
	threads[T_AVI]->signal_clr(SIG_VSENSE);
	threads[T_AVI]->signal_clr(SIG_APACE);
	threads[T_AVI]->signal_clr(SIG_VPACE);
}

void avi_thread(void)
{
	avi_state state = idle;
	osEvent event;
	Timer avi_timer;

	avi_timer.start();

	while(true) {
		event = Thread::signal_wait(0);
		switch (state) {

			case idle:
				if (event.value.signals & (SIG_APACE | SIG_ASENSE) {
					avi_timer.reset();
					state = avi;
					clear_own_signals();	
				}
				break;
						
			case avi:
				if (avi_timer.read() <= TIME_AVI) {
					if (event.value.signals & (SIG_FORCEVPACE)) {
						global_signal_set(SIG_VPACE);
						state = idle;
						clear_own_signals();
					}
				}
				else {
					if (clk < TIME_URI) {
						state = wait_uri;
					}
					else {
						global_signal_set(SIG_VPACE);
						state = idle;
					}
					clear_own_signals();
				}
				break;

			case wait_uri:
				// use randomness, but each case is a permutation
				// of the order in which we check each transition
				// to see if we can take it. if we just check one
				// transition in each case, we could potentially
				// not take a transition when we should
				// e.g. we receive SIG_FORCEVPACE but don't check it
				switch (lsfr % 6) {
					case 0: 
						if (event.value.signals & SIG_FORCEVPACE) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signal();
						}
						else if (event.value.signals & SIG_VSENSE) {
							state = idle;
							clear_own_signal();
						}
						else if (clk >= TIME_URI) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signals();
						}
						break;
					case 1: 
						if (event.value.signals & SIG_FORCEVPACE) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signal();
						}
						else if (clk >= TIME_URI) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signals();
						}
						else if (event.value.signals & SIG_VSENSE) {
							state = idle;
							clear_own_signal();
						}
						break;
					case 2: 
						if (event.value.signals & SIG_VSENSE) {
							state = idle;
							clear_own_signal();
						}
						else if (event.value.signals & SIG_FORCEVPACE) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signal();
						}
						else if (clk >= TIME_URI) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signals();
						}
						break;
					case 3: 
						if (event.value.signals & SIG_VSENSE) {
							state = idle;
							clear_own_signal();
						}
						else if (clk >= TIME_URI) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signals();
						}
						else if (event.value.signals & SIG_FORCEVPACE) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signal();
						}
						break;
					case 4: 
						if (clk >= TIME_URI) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signals();
						}
						else if (event.value.signals & SIG_FORCEVPACE) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signal();
						}
						else if (event.value.signals & SIG_VSENSE) {
							state = idle;
							clear_own_signal();
						}
						break;
					case 5: 
						if (clk >= TIME_URI) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signals();
						}
						else if (event.value.signals & SIG_VSENSE) {
							state = idle;
							clear_own_signal();
						}
						else if (event.value.signals & SIG_FORCEVPACE) {
							global_signal_set(SIG_VPACE);
							state = idle;
							clear_own_signal();
						}
						break;
				}
				break;
		}
	}
}
