// pvarp.cpp
#include "pvarp.h"
#include "common.h"

enum pvarp_state {
	idle,
	pvarp,
};

void pvarp_thread(void)
{
	pvarp_state state = idle;
	osEvent event;
	Timer pvarp_timer;

	pvarp_timer.start();

	while (true) {
		event = Thread::signal_wait(0, SIGNAL_TIMEOUT);
		switch (state) {
			case idle:
				if (event.value.signals & (SIG_VSENSE | SIG_VPACE)) {
					state = pvarp;
					pvarp_timer.reset();
				} else if (event.value.signals & SIG_AGET) {
					global_signal_set(SIG_ASENSE);
					//safe_println("ASENSE");
				}
				break;
			case pvarp:
				if (pvarp_timer.read_ms() > TIME_PVARP) {
					state = idle;
				}
				break;
			default:
				safe_println("Illegal PVARP state: %d", state);
				while (true); // halt VRP thread
		}
	}
}

