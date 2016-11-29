// external_signals.h
#ifndef EXTERNAL_SIGNALS_H
#define EXTERNAL_SIGNALS_H

//void flip_led(uint8_t led);
void vpace_received(void);
void apace_received(void);
void reset_vget(void const* args);
void reset_aget(void const* args);
void set_vget(void);
void set_aget(void);
void external_signals_thread(void);

#endif // EXTERNAL_SIGNALS_H