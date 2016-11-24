#ifndef COMMON_H
#define COMMON_H

#include <mbed.h>
#include <rtos.h>

#define DEBUG // enable printf

// signal difinitions
#define SIG_VSENSE 0x1 << 1
#define SIG_ASENSE 0x1 << 2
#define SIG_AGET 0x1 << 3
#define SIG_VGET 0x1 << 4
#define SIG_APACE 0x1 << 5
#define SIG_VPACE 0x1 << 6
#define SIG_FORCEAPACE 0x1 << 7
#define SIG_FORCEVPACE 0x1 << 8

// pin definitions
#define AGET_PIN

// Serial communication with PC
extern Serial pc;
extern Mutex printf_mutex;
void safe_print(char const* fmt...);
void safe_endl(void);
void safe_println(char const* fmt...);

// External pin interface.
// Can be changed later. Refer to mBed pinout.
extern InterruptIn Aget;
extern InterruptIn Vget;
extern DigitalOut Apace;
extern DigitalOut Vpace;
extern PwmOut speaker;

// heart sensing/pacing LEDs
#define LED_AGET 0
#define LED_VGET 1
#define LED_APACE 2
#define LED_VPACE 3

#define NUM_LEDS 4

extern DigitalOut leds[NUM_LEDS];

// Pacemaker timing criteria
// TODO: add more and change values later
#define TIME_VRP 100
#define TIME_PVARP 100
#define TIME_LRI 500
#define TIME_URI 100
#define TIME_AVI 100

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
#define T_EXTERNAL_SIGNALS 7

// signal setting/clearing
void global_signal_set(uint32_t signals);
void global_signal_clear(uint32_t signals);
void clear_own_signals(uint8_t pid);
extern Mutex signals_mutex;

// Global clock
extern Timer clk;

// Functions for controlling speaker/buzzer
void speaker_play_high(void);
void speaker_stop_high(void);
void speaker_play_low(void);
void speaker_stop_low(void);

#define SPEAKER_LOW_PERIOD 2000 // 500 us
#define SPEAKER_HIGH_PERIOD 800 // 800 us

// randomness
extern uint16_t lfsr_value;
void seed_lfsr(void);
uint16_t lfsr(void);

// Heart rate display
extern uint8_t obs_interval;

#endif // COMMON_H
