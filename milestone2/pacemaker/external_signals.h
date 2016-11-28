// external_signals.h
#ifndef EXTERNAL_SIGNALS_H
#define EXTERNAL_SIGNALS_H

void flip_led(uint8_t led);
void vget_received(void);
void aget_received(void);
void reset_vpace(void const* args);
void reset_apace(void const* args);
void set_vpace(void);
void set_apace(void);
void external_signals_thread(void);

#endif // EXTERNAL_SIGNALS_H

