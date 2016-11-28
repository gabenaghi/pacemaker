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
		event = Thread::signal_wait(SIG_VSENSE | SIG_VPACE | SIG_AGET);
		switch (state) {
			case idle:
				if (event.value.signals & SIG_VSENSE || 
						event.value.signals & SIG_VPACE) {
					state = pvarp;
					pvarp_timer.reset();
				} else if (event.value.signals & SIG_AGET) {
					global_signal_set(SIG_ASENSE);
				}
				break;
			case pvarp:
				if (pvarp_timer.read_ms() > TIME_PVARP) {
					state = idle;
				}
				break;
			default:
				pc.printf("Illegal PVARP state: %d\r\n", state);
				while (true); // halt VRP thread
		}
		clear_own_signals(T_PVARP);
	}
}

