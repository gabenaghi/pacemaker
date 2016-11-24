// common.cpp
#include "common.h"

Serial pc(USBTX, USBRX);

InterruptIn Aget(p5);
InterruptIn Vget(p6);
DigitalOut Apace(p7);
DigitalOut Vpace(p8);
PwmOut speaker(p26);

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

void global_signal_set(uint32_t signals)
{
	signals_mutex.lock();
	for (int i = 0; i < NUM_THREADS;) {
		//threads[i].signal_set(signals);
		safe_println("i = %d", i);
		i++;
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
	AnalogIn ain(p20);
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
	if (pid > NUM_THREADS) return;
	threads[pid].signal_clr(0xFFFFFFFF);
}

void speaker_play_low(void)
{
	speaker.period_ms(SPEAKER_LOW_PERIOD);
	speaker.write(0.50f); // 50% duty cycle
}	

void speaker_stop_low(void)
{
	speaker.write(0.0f); // 0% duty cycle
}

void speaker_play_high(void)
{
	speaker.period_ms(SPEAKER_HIGH_PERIOD);
	speaker.write(0.50f); // 50% duty cycle
}

void speaker_stop_high(void)
{
	speaker.write(0.0f); // 0% duty cycle
}

void safe_print(char const* fmt...)
{
	#ifdef DEBUG
	printf_mutex.lock();
	pc.printf(fmt);
	printf_mutex.unlock();
	#endif
}

void safe_endl(void)
{
	safe_print("\r\n");
}

void safe_println(char const* fmt...)
{
	safe_print(fmt);
	safe_endl();
}
