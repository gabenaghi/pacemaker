#include <mbed.h>
#include <rtos.h>

#ifndef COMMON_H
#define COMMON_H

// signal difinitions
#define SIG_VSENSE 0
#define SIG_ASENSE 1



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

// threads
extern Thread threads[];
#define NUM_THREADS 10
#define T_LRI 0
#define T_AVI 1
#define T_URI 2
#define T_PVARP 3
#define T_VRP 4
#define T_KEYBOARD 5
#define T_HEART_RATE_DISPLAY 6
#define T_INPUT_SIGNALS 7

#endif // COMMON_H
