#include <Arduino.h>
#include "ISRs.h"

static void ISR_clic(bool clic, uint32_t &interrupt_time, uint32_t &last_interrupt_time){
  interrupt_time = millis();
  if( (interrupt_time - last_interrupt_time) > BOUNCING_DELAY ) {
    clic=true;
  }
  last_interrupt_time = interrupt_time;
}