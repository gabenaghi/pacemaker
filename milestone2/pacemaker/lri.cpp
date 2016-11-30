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
		if (clk.read_ms() > TIME_LRI) {
			speaker_play_low();
		}
		event = Thread::signal_wait(0, SIGNAL_TIMEOUT);

		switch (state) {
			case lri:
				if (event.value.signals & SIG_FORCEAPACE) {
					global_signal_set(SIG_APACE);
					lri_timer.reset();
					clear_own_signals(T_LRI);
				}
				// NOTE: the below is different from milestone1 UPPAAL model.
				// in that model, transition with Vpace? doesn't update speaker
				// but i figure it should. so i changed it here
                // and in UPPAAL
				// NOTE: i was wrong
				else if (event.value.signals & (SIG_VSENSE | SIG_VPACE)) {
					// only update speaker on Vsense, not Vpace
					if (event.value.signals & SIG_VSENSE) {
						speaker_stop_low();
					}
					lri_timer.reset();
					clear_own_signals(T_LRI);
				}
				else if (event.value.signals & SIG_ASENSE) {
					state = ASed;
					clear_own_signals(T_LRI);
				}
				else if (lri_timer.read_ms() > TIME_LRI - TIME_AVI) {
					global_signal_set(SIG_APACE);
					//safe_println("Apace in LRI");
					lri_timer.reset();
					clear_own_signals(T_LRI);
				}
				break;

			case ASed:
				if (event.value.signals & SIG_FORCEAPACE) {
					global_signal_set(SIG_APACE);
					lri_timer.reset();
					state = lri;
					clear_own_signals(T_LRI);
				}
				// NOTE: the below is different from milestone1 UPPAAL model.
				// in that model, transition with Vpace? doesn't update speaker
				// but i figure it should. so i changed it here
                // and in UPPAAL
				// NOTE: i was wrong
				else if (event.value.signals & (SIG_VSENSE | SIG_VPACE)) {
					// only update speaker on Vsense, not Vpace
					if (event.value.signals & SIG_VSENSE) {
						speaker_stop_low();
					}
					state = lri;
					clear_own_signals(T_LRI);
				}
				break;
		}
	}
}


