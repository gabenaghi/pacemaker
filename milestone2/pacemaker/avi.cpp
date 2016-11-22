// avi.cpp
#include "common.h"
#include "avi.h"

// idle and avi states "done"
// TODO: wait_uri, and other TODOs below

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
				if (avi_timer.read() <= TAVI) {
					if (event.value.signals & (SIG_FORCEVPACE)) {
						// send VPACE
						// TODO
						state = idle;
						clear_own_signals();
					}
				}
				else {
					if (clk < TURI) {
						state = wait_uri;
					}
					else {
						// send VPACE
						// TODO
						state = idle;
					}
					clear_own_signals();
				}
				break;

			case wait_uri:
				break;
		}
	}
}
