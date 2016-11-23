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
        // forceVpace takes precedence, so check that first
        // we also want to go to idle if we get a Vsense,
        // so check that next.
        // only try to take other transitions after checking others
				if (event.value.signals & (SIG_FORCEVPACE)) {
					global_signal_set(SIG_VPACE);
					state = idle;
					clear_own_signals();
				}
        else if (event.value.signals & SIG_VSENSE) {
          state = idle;
          clear_own_signals;
        }
				else if (avi_timer.read() > TIME_AVI) {
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
				if (event.value.signals & (SIG_FORCEVPACE)) {
					global_signal_set(SIG_VPACE);
					state = idle;
					clear_own_signals();
				}
        else if (event.value.signals & SIG_VSENSE) {
          state = idle;
          clear_own_signals;
        }
				else if (clk >= TIME_URI) {
					global_signal_set(SIG_VPACE);
					state = idle;
					clear_own_signals();
				}
        break;
		}
	}
}
