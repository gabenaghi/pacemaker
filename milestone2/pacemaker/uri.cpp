// uri.cpp
#include "common.h"
#include "uri.h"

void uri_thread(void)
{
	osEvent event;
    bool just_started = true;

	while(true) {
        if (clk.read_ms() > TIME_LRI) {
            speaker_play_low();
        }
		event = Thread::signal_wait(0, SIGNAL_TIMEOUT);
        if (event.value.signals & (SIG_VPACE | SIG_VSENSE)) {
            if (just_started) {
                just_started = false;
            } else {
                uint32_t clk_val = clk.read_ms();
                safe_println("URI: clk_val = %d", clk_val);
                if (clk_val < TIME_URI) {
                    speaker_play_high();
                } else if (clk_val <= TIME_LRI) {
                    speaker_stop(); 
                }
            }
            if (event.value.signals & SIG_VSENSE) {
                clk.reset();
            }
        }
    }
}

