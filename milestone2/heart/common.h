#ifndef HEART_COMMON_H
#define HEART_COMMON_H

#include <mbed.h>
#include <rtos.h>

extern Serial pc;

// signal difinitions
#define SIG_VSIGNAL 0x1 << 0
#define SIG_ASIGNAL 0x1 << 1
#define SIG_APACE 0x1 << 2
#define SIG_VPACE 0X1 << 3

// threads
extern Thread threads[];
#define NUM_THREADS 5
#define T_GENERATOR 0
#define T_RESPONDER 1
#define T_LED 2
#define T_EXTERNAL 3
#define T_DISPLAY 4

// signal setting/clearing
void global_signal_set(uint32_t signals);
void global_signal_clear(uint32_t signals);
void clear_own_signals(uint8_t pid);
extern Mutex signals_mutex;

//pin allocations- arbitrary
#define VPACE_PIN p10
#define APACE_PIN p11
#define AGET_PIN p12
#define VGET_PIN p13

//gloabal i/o based signals
extern InterruptIn Apace;
extern InterruptIn Vpace;
extern DigitalOut Aget;
extern DigitalOut Vget;

//LED declarations
#define APACE_LED 0
#define VPACE_LED 1
#define APACE_HEART_LED 2
#define VPACE_HEART_LED 3
#define NUM_LEDS 4
extern DigitalOut leds[NUM_LEDS];

//timing parameters
#define ATOMIC_TIME 1 //ms
#define SIG_TIMEOUT 2
#define MS_PER_MINUTE 1000 * 60
#define minwait_V 149 //ms
#define minwait_A 149 //ms

//display stuff
#define OBSERVATION_MIN 1000
extern int Acount;
extern int Vcount;

//debug
#define TRACE 0

#endif //heart_common_h