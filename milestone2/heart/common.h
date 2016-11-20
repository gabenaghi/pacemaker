#ifndef HEART_COMMON_H
#define HEART_COMMON_H

#include <mbed.h>
#include <rtos.h>

// signal difinitions
#define SIG_VSIGNAL 0
#define SIG_ASIGNAL 1

// threads
extern Thread threads[];
#define NUM_THREADS 10
#define T_GENERATOR 0

#endif //heart_common_h