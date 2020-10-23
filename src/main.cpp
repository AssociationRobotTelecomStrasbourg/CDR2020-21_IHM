#include <Arduino.h>
#include "LiquidCrystal_I2C.h"

#include "debug.h"
#include "encoder.h"
#include "menus.h"



//var
//Variables liées au clic de l'encodeur
bool clic=0;
//Varible d'anti-rebond
uint32_t interrupt_time;
uint32_t last_interrupt_time;
uint32_t bouncing_delay;

int posEncoder;
bool rot;
bool refreshScreen;
uint8_t menuA;
uint8_t menuB;

bool menu0; //affichage du menu principal
bool menuGO; //affichage du menu d'attente de départ
bool menuTest; //affichage du menu test
bool go; //Attente début du match
bool gone; //Indicateur début du match (0 si en attente, 1 si commencé)
void affichageMenu0();
void affichageMenuGo();
//Proto ISR
void ISR_clic();
void ISR_encoder();

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

float tension_batterie;
int tension_batterie_raw;
int i = 0;
uint16_t msg = 0;

uint8_t N_lidars_fixes;

void setup(){
  Serial.begin(9600);
  Serial.flush();
  //Variables d'état pour navigation menu
  posEncoder = 0;

  bouncing_delay = 400;
  interrupt_time = 0;
  last_interrupt_time = 0;
  
  rot = false;
  refreshScreen = false;

  menu0 = false; //affichage du menu principal
  menuGO = false; //affichage du menu d'attente de départ
  menuTest = false; //affichage du menu test
  go = false; //affichage des infos du match
  gone = false;
  lcd.init();// initialize the lcd 

  lcd.backlight();
  lcd.setCursor(3,0);
  
  lcd.print("Attente GO STRT");
  delay(2000);

  lcd.setCursor(0,0);
  while(Serial.available()==0){
    //attente reception message
  }
  lcd.clear();

  msg = Serial.read();
  if(msg == 0x7E){
    //Reception du message OK STRT
    lcd.print("STRT OK");
  }
  else{
    lcd.print("PB INIT STRT");
  }
  lcd.setCursor(0,1);
  delay(100);

  msg = Serial.read();
  if(msg==0x43){
    lcd.print("MOVT OK");
  }
  else if(msg==0xEE){
    lcd.print("PB MOVT/STRT");
  }
  else{
    lcd.print("PB CONF MOVT");
  }

  lcd.setCursor(0,2);

  while(Serial.available()<2){
    //attente reception tension batterie
    #ifdef debug
      Serial.print("Available n: ");
      Serial.println(Serial.available());
    #endif
    delay(100);
  }
  tension_batterie_raw = (Serial.read()<<8)|Serial.read();
  #ifdef debug
    Serial.println("value: ");
    Serial.println(tension_batterie_raw);
  #endif
  tension_batterie = (tension_batterie_raw/1023.0)*3.3*4.0;//(Serial.read()<<8)|(Serial.read());
  lcd.print("Vbat: ");
  lcd.print(tension_batterie);
  lcd.print("V");

  while(Serial.available()<1){
    #ifdef debug
      Serial.print("Lidars fixes: ");
      Serial.println(Serial.available());
    #endif
    delay(100);
  }
  N_lidars_fixes = Serial.read();
  lcd.setCursor(0,3);
  lcd.print("N Lidars ok: ");
  lcd.print(N_lidars_fixes);
  digitalWrite(13,HIGH);
  delay(2000);
  digitalWrite(13,LOW);
  //Initialisation Encoder
  pinMode(SW, INPUT_PULLUP);
  pinMode(DT, INPUT);
  pinMode(CLK, INPUT);

  attachInterrupt(digitalPinToInterrupt(SW),ISR_clic,RISING);
  attachInterrupt(digitalPinToInterrupt(CLK),ISR_encoder,CHANGE);

  clic = 0;
}


void loop(){
  //0x66 vers strat -> attente tirette
  rotation(rot,posEncoder); //fonction de decodage de l'encodeur incremental
  menuMEF(posEncoder,clic,menuA,menuB,refreshScreen,lcd);
  displayCursor(posEncoder,menuA,lcd);


}
/* 
void affichageMenu0(){
  if(refresh){
    delay(200);
    #ifdef debug
      Serial.println("MENU 0");
    #endif
    refresh = 0;
    lcd.setCursor(1,0);
    lcd.print("GO Match");
    lcd.setCursor(1,1);
    lcd.print("Menu Tests");
  }
  lcd.cursor_on();
  lcd.setCursor(0,posEncoder);
  if(posEncoder == 0 && clic){
    menuGO = 1;
    menu0 = 0;
    refresh = 1;
  }
  else if(posEncoder == 1 && clic){
    menuTest = 1;
    menu0 = 0;
    refresh = 1;
  }
}

void affichageMenuGo(){
  if(refresh){
    refresh = 0;
    delay(200);
    lcd.clear();
    lcd.cursor_off();
    lcd.setCursor(6,1);
    lcd.print("Attente");
    lcd.setCursor(5,2);
    lcd.print("demarrage");
    Serial.write(0x66); //Signifie à la Teensy Strat qu'elle doit lire la valeur de la tirette de départ
    #ifdef debug
      Serial.println("MENU GO");
    #endif
  }
}
 */
void ISR_clic(){
  interrupt_time = millis();
  if( (interrupt_time - last_interrupt_time) > bouncing_delay ) {
    clic=true;
  }
  last_interrupt_time = interrupt_time;
}

void ISR_encoder(){
  rot = true;
}