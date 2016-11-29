#ifndef HEART_COMMON_H
#define HEART_COMMON_H

#include <mbed.h>
#include <rtos.h>

// signal difinitions
#define SIG_VSIGNAL 0x1 << 0
#define SIG_VSIGNAL_RES 0x1 << 1
#define SIG_ASIGNAL 0x1 << 2
#define SIG_ASIGNAL_RES 0x1 << 3

// threads
extern Thread threads[];
#define NUM_THREADS 10
#define T_GENERATOR 0
#define T_RESPONDER 1

//pin allocations- arbitrary
#define VPACE_PIN p10
#define APACE_PIN p11
#define AGET_PIN p12
#define VGET_PIN p13

//gloabal i/o based signals
extern DigitalIn Vpace;
extern DigitalIn Apace;

//LED declarations
#define APACE_LED 0
#define VPACE_LED 1
#define APACE_HEART_LED 2
#define VPACE_HEART_LED 3
extern DigitalOut LED[4];

//timing parameters
#define ATOMIC_TIME 1 //ms

//display stuff
#define OBSERVATION_MIN 1.0f
extern int Acount;
extern int Vcount;

#endif //heart_common_h