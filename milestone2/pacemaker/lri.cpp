// lri.cpp
#include "common.h"
#include "lri.h"

enum lri_state {
	lri,
	ASed,
}

void clear_own_signals()
{
	threads[T_LRI]->signal_clr(0xFFFFFFFF);
}

void lri_thread(void)
{
	lri_state state = idle;
	osEvent event;
	Timer lri_timer;

	lri_timer.start();

	while(true) {
		event = Thread::signal_wait(0);
		switch (state) {

			case lri:
				if (event.value.signals & SIG_FORCEAPACE) {
					global_signal_set(SIG_APACE)
					lri_timer.reset();
					clear_own_signals();
				}
				// NOTE: the below is different from milestone1 UPPAAL model.
				// in that model, transition with Vpace? doesn't update speaker
				// but i figure it should. so i changed it here
                // and in UPPAAL
				else if (event.value.signals & (SIG_VSENSE | SIG_VPACE)) {
					if (clk.read() < TIME_LRI) {
						speaker_play_low();
					}
					else {
						speaker_stop_low();
					}
          lri_timer.reset();
					clear_own_signals();
				}
				else if (event.value.signals & SIG_ASENSE) {
					state = ASed;
					clear_own_signals();
				}
				else if (lri_timer.read() > TIME_LRI - TIME_AVI) {
					global_signal_set(SIG_APACE);
					lri_timer.reset();
					clear_own_signals();
				}
				break;

			case ASed:
				if (event.value.signals & SIG_FORCEAPACE) {
					global_signal_set(SIG_APACE)
					lri_timer.reset();
					state = idle;
					clear_own_signals();
				}
				// NOTE: the below is different from milestone1 UPPAAL model.
				// in that model, transition with Vpace? doesn't update speaker
				// but i figure it should. so i changed it here
                // and in UPPAAL
				else if (event.value.signals & (SIG_VSENSE | SIG_VPACE)) {
					if (clk.read() < TIME_LRI) {
						speaker_play_low();
					}
					else {
						speaker_stop_low();
					}
					state = idle;
					clear_own_signals();
				}
				break;
		}
	}
}

