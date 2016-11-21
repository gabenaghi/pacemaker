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
		event = Thread::signal_wait(0);
		switch (state) {
			case idle:
				if (event.value.signals == SIG_VGET) {
					state = inter;
				} else if (event.value.signals == SIG_VPACE) {
					state = vrp;
					vrp_timer.reset();
				}
				break;
			case inter:
				state = vrp;
				vrp_timer.reset();
				// TODO: emit Vsense!
				break;
			case vrp:
				while (vrp_timer.read_ms <= TIME_VRP); // wait until t > TVRP
				state = idle;
				break;
			default: // illegal state
				pc.printf("Illegal VRP state: %d\r\n", state);
				while (true); // halt VRP thread
		}
	}
}