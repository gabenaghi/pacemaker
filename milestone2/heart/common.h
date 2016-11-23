#ifndef HEART_COMMON_H
#define HEART_COMMON_H

#include <mbed.h>
#include <rtos.h>

// signal difinitions
#define SIG_VSIGNAL 0x1 << 0
#define SIG_ASIGNAL 0x1 << 1

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

//timing parameters
#define ATOMIC_TIME 1 //ms

#endif //heart_common_h