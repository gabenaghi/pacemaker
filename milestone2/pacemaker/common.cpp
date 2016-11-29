// common.cpp
#include "common.h"

Serial pc(USBTX, USBRX);

InterruptIn Aget(AGET_PIN);
InterruptIn Vget(VGET_PIN);
DigitalOut Apace(APACE_PIN);
DigitalOut Vpace(VPACE_PIN);
PwmOut speaker(SPEAKER_PIN);

uint16_t TIME_LRI = 2000;
uint16_t TIME_URI = 100;

DigitalOut leds[NUM_LEDS] = {
	DigitalOut(LED1),
	DigitalOut(LED2),
	DigitalOut(LED3),
	DigitalOut(LED4),
};

Timer clk;

uint16_t lfsr_value;

uint8_t obs_interval = 10; // ms

Mutex printf_mutex;
Mutex signals_mutex;

// don't think this needs to be global, so not declared in common.h
// feel free to change if necessary
uint8_t speaker_status;

void global_signal_set(uint32_t signals)
{
	signals_mutex.lock();
	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i].signal_set(signals);
	}
	signals_mutex.unlock();
}

void global_signal_clear(uint32_t signals)
{
	signals_mutex.lock();
	for (uint8_t i = 0; i < NUM_THREADS; i++) {
		threads[i].signal_clr(signals);
	}
	signals_mutex.unlock();
}

void seed_lfsr(void) {
	AnalogIn ain(LFSR_PIN);
    lfsr_value = ain.read_u16();
}    

uint16_t lfsr(void) {
    uint16_t new_bit = (lfsr_value >> 15 & 0x0001) ^ (lfsr_value >> 13 & 0x0001) ^ 
                        (lfsr_value >> 12 & 0x0001) ^ (lfsr_value >> 10 & 0x0001);
    lfsr_value = (lfsr_value << 1) | new_bit;
    return lfsr_value;
}   

void clear_own_signals(uint8_t pid)
{
	// assuming indices are 0-indexed, this should be >=, not >
	if (pid >= NUM_THREADS) return;

	// probably good to grab mutex to ensure signal changes happen in order
	signals_mutex.lock();
	threads[pid].signal_clr(0xFFFFFFFF);
	signals_mutex.unlock();
}

void speaker_play_low(void)
{
	speaker.period_ms(SPEAKER_LOW_PERIOD);
	speaker.write(0.50f); // 50% duty cycle
    speaker_status = PLAYING_LOW;
}	

void speaker_stop_low(void)
{
	if (speaker_status == PLAYING_LOW) {
		speaker.write(0.0f); // 0% duty cycle
		speaker_status = NOT_PLAYING;
	}
}

void speaker_play_high(void)
{
	speaker.period_ms(SPEAKER_HIGH_PERIOD);
	speaker.write(0.50f); // 50% duty cycle
	speaker_status = PLAYING_HIGH;
}

void speaker_stop_high(void)
{
	if (speaker_status == PLAYING_HIGH) {
		speaker.write(0.0f); // 0% duty cycle
		speaker_status = NOT_PLAYING;
	}
}

