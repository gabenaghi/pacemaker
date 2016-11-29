#include "common.h"

Serial pc(USBTX,USBRX);

//Global signals taken in from the heart. 
InterruptIn Apace(APACE_PIN);//pin6
InterruptIn Vpace(VPACE_PIN);//pin5


DigitalOut leds[NUM_LEDS] = {
	DigitalOut(LED1),
	DigitalOut(LED2),
	DigitalOut(LED3),
	DigitalOut(LED4),
};

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

void clear_own_signals(uint8_t pid)
{
	// assuming indices are 0-indexed, this should be >=, not >
	if (pid >= NUM_THREADS) return;

	// probably good to grab mutex to ensure signal changes happen in order
	signals_mutex.lock();
	threads[pid].signal_clr(0xFFFFFFFF);
	signals_mutex.unlock();
}