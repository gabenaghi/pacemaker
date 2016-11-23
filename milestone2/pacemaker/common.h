#include <mbed.h>
#include <rtos.h>

#ifndef COMMON_H
#define COMMON_H

// signal difinitions
#define SIG_VSENSE 0x1 << 1
#define SIG_ASENSE 0x1 << 2
#define SIG_AGET 0x1 << 3
#define SIG_VGET 0x1 << 4
#define SIG_APACE 0x1 << 5
#define SIG_VPACE 0x1 << 6
#define SIG_FORCEAPACE 0x1 << 7
#define SIG_FORCEVPACE 0x1 << 8

// Serial communication with PC
Serial pc(USBTX, USBRX);

// External pin interface.
// Can be changed later. Refer to mBed pinout.
InterruptIn Aget(p15);
InterruptIn Vget(p16);
DigitalOut Apace(p17);
DigitalOut Vpace(p18);
PwmOut speaker(p26);

// heart sensing/pacing LEDs
#define LED_AGET 0
#define LED_VGET 1
#define LED_ASENSE 2
#define LED_VSENSE 3

#define NUM_LEDS 4

DigitalOut leds[NUM_LEDS] = {
	DigitalOut(LED1),
	DigitalOut(LED2),
	DigitalOut(LED3),
	DigitalOut(LED4),
};

// Pacemaker timing criteria
// TODO: add more and change values later
#define TIME_VRP 100;
#define TIME_PVARP 100;
#define TIME_URI 100;
#define TIME_AVI 100;

// threads
extern Thread* threads[];
#define NUM_THREADS 10
#define T_LRI 0
#define T_AVI 1
#define T_URI 2
#define T_PVARP 3
#define T_VRP 4
#define T_KEYBOARD 5
#define T_HEART_RATE_DISPLAY 6
#define T_EXTERNAL_SIGNALS 7

// Global signal setting/clearing
void global_signal_set(int32_t signals);
void global_signal_clear(int32_t signals);

// Global clock
extern Timer clk;

// Functions for controlling speaker/buzzer
// TODO: implement these
void speaker_play_high(void);
void speaker_stop_high(void);
void speaker_play_low(void);
void speaker_stop_low(void);

// randomness
uint16_t lfsr_value;
void seed_lfsr(void);
uint16_t lfsr(void);

#endif // COMMON_H
