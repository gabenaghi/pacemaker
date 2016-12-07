#include "common.h"

Serial pc(USBTX,USBRX);
uint32_t signal_times_ids[SIGNAL_TIMES_ARR_SIZE] = {0};
uint32_t signal_times_times[SIGNAL_TIMES_ARR_SIZE] = {0};
uint32_t signal_times_index = 0;
Timer signal_times_clk;

//Global signals taken in from the heart. 
InterruptIn Apace(APACE_PIN);//pin6
InterruptIn Vpace(VPACE_PIN);//pin5
Mutex signals_mutex;

DigitalOut leds[NUM_LEDS] = {
	DigitalOut(LED1),
	DigitalOut(LED2),
	DigitalOut(LED3),
	DigitalOut(LED4),
};

void global_signal_set(uint32_t signals)
{
	signals_mutex.lock();
	for (int i = 0; i < NUM_THREADS; ++i) {
		threads[i].signal_set(signals);
		//printf("i = %d", i);
	}
	uint8_t index = 0;
	switch (signals) {
		case SIG_VSIGNAL:
			index = SIG_VSIGNAL_ID;
			break;
		case SIG_ASIGNAL:
			index = SIG_ASIGNAL_ID;
			break;
		case SIG_APACE:
			index = SIG_APACE_ID;
			break;
		case SIG_VPACE:
			index = SIG_VPACE_ID;
			break;
	}
	signal_times_ids[signal_times_index] = index;
	signal_times_times[signal_times_index] = signal_times_clk.read_ms();
	signal_times_index++;
	if (signal_times_index >= SIGNAL_TIMES_ARR_SIZE) {
		signal_times_index = 0; // wraparound
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

void dump_signal_times(void)
{
	uint32_t i = 0;
	while (i < signal_times_index) {
		pc.printf("Signal: %s, Time: %d\r\n", get_signal_name(signal_times_ids[i]), signal_times_times[i]);
		i++;
	}
}

const char* get_signal_name(uint32_t signals) {
	switch (signals) {
		case SIG_VSIGNAL_ID:
			return "Vsignal";
			break;
		case SIG_ASIGNAL_ID:
			return "Asignal";
			break;
		case SIG_APACE_ID:
			return "Apace";
			break;
		case SIG_VPACE_ID:
			return "Vpace";
			break;
		default:
			return "INVALID SIGNAL";
			break;
	}
}

