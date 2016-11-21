#include <mbed.h>
#include <rtos.h>

#ifndef COMMON_H
#define COMMON_H

// signal difinitions
#define SIG_VSENSE 0
#define SIG_ASENSE 1

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

#endif // COMMON_H
