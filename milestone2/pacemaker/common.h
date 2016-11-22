#include <mbed.h>
#include <rtos.h>

#ifndef COMMON_H
#define COMMON_H

// signal difinitions
#define SIG_VSENSE 1
#define SIG_ASENSE 2
#define SIG_AGET 4
#define SIG_VGET 8
#define SIG_APACE 16
#define SIG_VPACE 32

// Serial communication with PC
Serial pc(USBTX, USBRX);

// External pin interface.
// Can be changed later. Refer to mBed pinout.
DigitalOut Aget(p15);
DigitalOut Vget(p16);
DigitalOut Asense(p17);
DigitalOut Vsense(p18);
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
#define T_INPUT_SIGNALS 7

// Global signal setting/clearing
void global_signal_set(int32_t signals);
void global_signal_clear(int32_t signals);

// randomness
uint16_t lfsr_value;
void seed_lfsr(void);
uint16_t lfsr(void);

#endif // COMMON_H
