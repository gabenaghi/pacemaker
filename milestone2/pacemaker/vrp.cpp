// vrp.cpp
#include "common.h"
#include "vrp.h"

enum vrp_state {
	idle,
	vrp,
};

void vrp_thread(void)
{
	vrp_state state = idle;
	osEvent event;
	Timer vrp_timer;

	vrp_timer.start();

	while (true) {
		switch (state) {
			case idle:
				event = Thread::signal_wait(SIG_VGET | SIG_VPACE);
				if (event.value.signals & SIG_VGET) {
					state = vrp;
					vrp_timer.reset();
					global_signal_set(SIG_VSENSE);
				} else if (event.value.signals & SIG_VPACE) {
					state = vrp;
					vrp_timer.reset();
				}
				break;
			case vrp:
				if (vrp_timer.read_ms() > TIME_VRP) {
					state = idle;
				}
				break;
			default: // illegal state
				safe_println("Illegal VRP state: %d", state);
				while (true); // halt VRP thread
		}
		clear_own_signals(T_VRP);
	}
}

