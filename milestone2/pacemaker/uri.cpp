// uri.cpp
#include "common.h"
#include "uri.h"

void uri_thread(void)
{
	osEvent event;
    bool just_started = true;

	while(true) {
		event = Thread::signal_wait(0, SIGNAL_TIMEOUT);
        if (event.value.signals & (SIG_VPACE | SIG_VSENSE)) {
            if (just_started) {
                just_started = false;
            }
            else if (clk.read_ms() < TIME_URI) {
                speaker_play_high();
            }
            else {
                speaker_stop_high(); 
            }
            clk.reset();
            clear_own_signals(T_URI);
        }
    }
}

