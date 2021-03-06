#ifndef HEART_COMMON_H
#define HEART_COMMON_H

#include <mbed.h>
#include <rtos.h>

extern Serial pc;

// signal difinitions
#define SIG_VSIGNAL_ID 0
#define SIG_ASIGNAL_ID 1
#define SIG_APACE_ID 2
#define SIG_VPACE_ID 3
#define SIG_VSIGNAL 0x1 << SIG_VSIGNAL_ID
#define SIG_ASIGNAL 0x1 << SIG_ASIGNAL_ID
#define SIG_APACE 0x1 << SIG_APACE_ID
#define SIG_VPACE 0X1 << SIG_VPACE_ID

#define SIGNAL_TIMES_ARR_SIZE 128
extern uint32_t signal_times_ids[SIGNAL_TIMES_ARR_SIZE];
extern uint32_t signal_times_times[SIGNAL_TIMES_ARR_SIZE];
extern uint32_t signal_times_index;
extern Timer signal_times_clk;

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
#define TEST_START_TIMEOUT 5000//ms
#define SIG_TIMEOUT 2
#define MS_PER_MINUTE 1000 * 60
#define minwait_V 149 //ms
#define minwait_A 149 //ms
#define TIME_VRP 400
#define TIME_PVARP 300
#define TIME_LRI 2000
#define TIME_URI 400 // which value should we use?
#define TIME_AVI 150
#define TOLERANCE 20
#define TWO_TOLERANCE 2 * TOLERANCE 

//display stuff
#define OBSERVATION_MIN 1000
extern int Acount;
extern int Vcount;

//debug
#define TRACE 0

void dump_signal_times(void);
const char* get_signal_name(uint32_t signals);

extern char keypress;

void update_keypress();

void clear_keypress();

#endif //heart_common_h