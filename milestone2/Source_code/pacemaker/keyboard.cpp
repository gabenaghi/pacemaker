// keyboard.cpp
#include "keyboard.h"
#include "common.h"

void keyboard_thread(void) 
{
	char input = ' ';
	bool setting_obs_interval = false;
	char number_string[4];
	short int curr_index = 0;
	bool manual = false;

	while (true) {
		while (!pc.readable()) {
			Thread::yield();
		}
		input = pc.getc();
		if (setting_obs_interval) {
			if ('0' <= input && input <= '9') {
				if (curr_index < 3) {
					number_string[curr_index] = input;
					++curr_index;
				}
				else {
					// shift chars in array to left by 1
					number_string[0] = number_string[1];
					number_string[1] = number_string[2];
					// add new char
					number_string[2] = input;
				}
			}
			else if (input == '\n' || input == '\r') {
				// stop accepting numbers, process string into number
				number_string[curr_index] = '\0';
				int converted = atoi(number_string);
				// if converted is 0 or can't fit in 8 bits, ignore it
				// otherwise, update obs_interval
				if (converted && converted < 256) {
					obs_interval = (uint8_t)converted;
				}

				setting_obs_interval = false;
			}
		}
		else {
			switch (input)  {
				// normal mode
				case 'n':
				case 'N':
					// should we add a mutex for these time variables?
					TIME_URI = 400;
					TIME_LRI = 1000;
					manual = false;
					break;

				// sleep mode
				case 's':
				case 'S':
					TIME_URI = 500;
					TIME_LRI = 1200;
					manual = false;
					break;

				// exercise mode
				case 'e':
				case 'E':
					TIME_URI = 300;
					TIME_LRI = 800;
					manual = false;
					break;

				// manual mode
				case 'm':
				case 'M':
					if (!manual) {
						TIME_URI = 300;
						TIME_LRI = 1200;
						manual = true;
					} else {
						TIME_URI = 400;
						TIME_LRI = 1000;
						manual = false;
					}
					break;

				// send manual Apace
				case 'a':
				case 'A':
					if (manual) {
						global_signal_set(SIG_FORCEAPACE);
						//safe_println("Forced an Apace");
					}
					break;

				// send manual Vpace
				case 'v':
				case 'V':
					if (manual) {
						global_signal_set(SIG_FORCEVPACE);
						//safe_println("Forced a Vpace");
					}
					break;

				// start setting observation interval
				case 'o':
				case 'O':
					setting_obs_interval = true;
					curr_index = 0;
					break;
			}
		}
	}
}

