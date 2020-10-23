#include <Arduino.h>
#include "encoder.h"

void rotation(bool &rot, int &posEncoder){
    if(rot){
    rot = false;
    #ifdef debug
      Serial.print("Enc valeur: ");
      Serial.println(posEncoder);
    #endif
    if(digitalRead(DT) == digitalRead(CLK)){
      posEncoder+=1;
    }
    else{
      posEncoder-=1;
    }
  }
}