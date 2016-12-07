// lri.cpp
#include "common.h"
#include "lri.h"

enum lri_state {
	lri,
	ASed,
};

void lri_thread(void)
{
	lri_state state = lri;
	osEvent event;
	Timer lri_timer;

	lri_timer.start();

	while(true) {
		event = Thread::signal_wait(0, SIGNAL_TIMEOUT);

		switch (state) {
			case lri:
				if (event.value.signals & SIG_FORCEAPACE) {
					global_signal_set(SIG_APACE);
				}
				// NOTE: the below is different from milestone1 UPPAAL model.
				// in that model, transition with Vpace? doesn't update speaker
				// but i figure it should. so i changed it here
                // and in UPPAAL
				// NOTE: i was wrong
				// NOTE: all speaker changes now handled in URI, not LRI.
				else if (event.value.signals & (SIG_VSENSE | SIG_VPACE)) {
					lri_timer.reset();
					//safe_println("LRI Timer Reset in LRI.LRI");
				}
				else if (event.value.signals & SIG_ASENSE) {
					state = ASed;
				}
				else if (lri_timer.read_ms() > TIME_LRI - TIME_AVI) {
					//safe_println("Atrium Paced in LRI.LRI");
					global_signal_set(SIG_APACE);
					state = ASed;
				}
				break;

			case ASed:
				if (event.value.signals & SIG_FORCEAPACE) {
					global_signal_set(SIG_APACE);
					state = lri;
				}
				// NOTE: the below is different from milestone1 UPPAAL model.
				// in that model, transition with Vpace? doesn't update speaker
				// but i figure it should. so i changed it here
                // and in UPPAAL
				// NOTE: i was wrong
				else if (event.value.signals & (SIG_VSENSE | SIG_VPACE)) {
					state = lri;
					lri_timer.reset();
					//safe_println("LRI Timer Reset in LRI.Ased");
				}
				break;
		}
	}
}


