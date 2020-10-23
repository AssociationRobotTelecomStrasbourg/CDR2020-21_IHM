#ifndef ISRS_H
#define ISRS_H
#include <Arduino.h>

#define BOUNCING_DELAY 200

//Proto ISR
static void ISR_clic(bool clic, uint32_t &interrupt_time, uint32_t &last_interrupt_time);
void ISR_encoder();

#endif


