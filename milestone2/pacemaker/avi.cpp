// avi.cpp
#include "common.h"
#include "avi.h"

enum avi_state {
	idle,
	avi,
	wait_uri,
};

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
				if (event.value.signals & (SIG_APACE | SIG_ASENSE)) {
					avi_timer.reset();
					state = avi;
					clear_own_signals(T_AVI);	
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
					clear_own_signals(T_AVI);
				}
        else if (event.value.signals & SIG_VSENSE) {
          state = idle;
          clear_own_signals(T_AVI);
        }
				else if (avi_timer.read() > TIME_AVI) {
					if (clk.read() < TIME_URI) {
						state = wait_uri;
					}
					else {
						global_signal_set(SIG_VPACE);
						state = idle;
					}
					clear_own_signals(T_AVI);
				}
				break;

			case wait_uri:
				if (event.value.signals & (SIG_FORCEVPACE)) {
					global_signal_set(SIG_VPACE);
					state = idle;
					clear_own_signals(T_AVI);
				}
        else if (event.value.signals & SIG_VSENSE) {
          state = idle;
          clear_own_signals(T_AVI);
        }
				else if (clk.read() >= TIME_URI) {
					global_signal_set(SIG_VPACE);
					state = idle;
					clear_own_signals(T_AVI);
				}
        break;
	}
}
}
