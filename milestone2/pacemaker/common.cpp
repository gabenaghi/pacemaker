// common.cpp
#include "common.h"

void global_signal_set(uint32_t signals)
{
	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i]->signal_set(signals);
	}
}

void global_signal_clear(uint32_t signals)
{
	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i]->signal_clr(signals);
	}
}

void seed_lfsr(void) {
	AnalogIn ain(p20);
    lfsr_value = ain.read_u16();
}    

uint16_t lfsr(void) {
    uint16_t new_bit = (lfsr_value >> 15 & 0x0001) ^ (lfsr_value >> 13 & 0x0001) ^ 
                        (lfsr_value >> 12 & 0x0001) ^ (lfsr_value >> 10 & 0x0001);
    lfsr_value = (lfsr_value << 1) | new_bit;
    return lfsr_value;
}   
