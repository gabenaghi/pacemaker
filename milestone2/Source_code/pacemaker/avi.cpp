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
		event = Thread::signal_wait(0, SIGNAL_TIMEOUT);
		switch (state) {

			case idle:
				if (event.value.signals & (SIG_FORCEVPACE)) {
					global_signal_set(SIG_VPACE);
				}
				else if (event.value.signals & (SIG_APACE | SIG_ASENSE)) {
					avi_timer.reset();
					state = avi;
					//safe_println("IDLE -> AVI");	
				}
				break;
						
			case avi:
        // forceVpace takes precedence, so check that first
        // we also want to go to idle if we get a Vsense,
        // so check that next.
        // only try to take other transitions after checking others
				if (event.value.signals & (SIG_FORCEVPACE)) {
					global_signal_set(SIG_VPACE);
					//safe_println("FORCE VPACE in AVI 1");
					state = idle;
					//safe_println("AVI -> IDLE");
				}
				else if (event.value.signals & SIG_VSENSE) {
					state = idle;
					//safe_println("AVI -> IDLE (VSENSE)");
				}
				else if (avi_timer.read_ms() > TIME_AVI) {
					if (clk.read_ms() < TIME_URI) {
						state = wait_uri;
						//safe_println("AVI -> WAIT_URI");
					}
					else {
						global_signal_set(SIG_VPACE);
						//safe_println("VPACE in AVI 1");
					    //safe_println("AVI -> IDLE (VPACE!)");
						state = idle;
					}
				}
				break;

			case wait_uri:
				if (event.value.signals & (SIG_FORCEVPACE)) {
					global_signal_set(SIG_VPACE);
					//safe_println("FORCE VPACE in AVI 2");
					state = idle;
				}
				else if (event.value.signals & SIG_VSENSE) {
					state = idle;
				}
				else if (clk.read_ms() >= TIME_URI) {
					//safe_println("VPACE in AVI 2");
					global_signal_set(SIG_VPACE);
					state = idle;
				}
        		break;
		}
	}
}

