#ifndef HEART_COMMON_H
#define HEART_COMMON_H

#include <mbed.h>
#include <rtos.h>

// signal difinitions
#define SIG_VSIGNAL 0x1 << 0
#define SIG_VSIGNAL_RES 0x1 << 1
#define SIG_ASIGNAL 0x1 << 2
#define SIG_ASIGNAL_RES 0x1 << 3
#define SIG_APACE 0x1 << 4
#define SIG_VPACE 0X1 << 5

// threads
extern Thread threads[];
#define NUM_THREADS 4
#define T_GENERATOR 0
#define T_RESPONDER 1
<<<<<<< HEAD
#define T_LED 2
#define T_EXTERNAL 3

// signal setting/clearing
void global_signal_set(uint32_t signals);
void global_signal_clear(uint32_t signals);
void clear_own_signals(uint8_t pid);
extern Mutex signals_mutex;
=======
#define T_DISPLAY 2
>>>>>>> 14b199becf1d7881c0f1c705bac30c3fea2a8f68

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
extern DigitalOut LED[NUM_LEDS];

//timing parameters
#define ATOMIC_TIME 1 //ms

//display stuff
#define OBSERVATION_MIN 1.0f
extern int Acount;
extern int Vcount;

#endif //heart_common_h