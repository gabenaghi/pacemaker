// vrp.cpp
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

	while (true) {
		event = Thread::signal_wait(0);
		switch (state) {
			case idle:
				

			case inter:
			case vrp:
			default:

		}
	}
}