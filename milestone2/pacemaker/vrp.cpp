// vrp.cpp
#include "common.h"
#include "vrp.h"

enum vrp_state {
	idle,
	inter,
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
					state = inter;
				} else if (event.value.signals & SIG_VPACE) {
					state = vrp;
					vrp_timer.reset();
				}
				break;
			case inter:
				state = vrp;
				vrp_timer.reset();
				global_signal_set(SIG_VSENSE);
				break;
			case vrp:
				while (vrp_timer.read_ms <= TIME_VRP); // wait until t > TVRP
				state = idle;
				break;
			default: // illegal state
				pc.printf("Illegal VRP state: %d\r\n", state);
				while (true); // halt VRP thread
		}
		threads[T_VRP]->signal_clr(0xFFFFFFFF);
	}
}
