#include <Arduino.h>
#include "LiquidCrystal_I2C.h"

#define _debug 
//Définition pins encoder
#define SW 3
#define DT 4
#define CLK 2

//var
bool clic;
int posEncoder;
bool rotation;
bool refresh;

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

  clic = false;

  rotation = false;
  refresh = false;

  menu0 = false; //affichage du menu principal
  menuGO = false; //affichage du menu d'attente de départ
  menuTest = false; //affichage du menu test
  go = false; //affichage des infos du match
  gone = false;
  lcd.init();                      // initialize the lcd 

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

  #ifdef debug
    Serial.print("Lecture CLIC pre-loop1: ");
    Serial.println(digitalRead(SW));
    delay(400);
    Serial.print("Lecture CLIC pre-loop2: ");
    Serial.println(digitalRead(SW));
    delay(400);
  #endif

  while(digitalRead(SW)){
    //attente du clic avant de continuer
    #ifdef debug
      Serial.print("Lecture SW|CLIC: ");
      Serial.print(digitalRead(SW));
      Serial.print("|");
      Serial.println(clic);
      delay(400);
    #endif
  }
}


void loop(){
  if(posEncoder > 3){
    posEncoder = 3;
  }
  else if(posEncoder <0){
    posEncoder = 0;
  }
  if(clic && !menu0 && !menuGO){
    refresh = true;
    menu0 = true;
    clic = false;
    lcd.clear();
  }

  if(menu0){
    affichageMenu0();
  }

  if(menuGO){
    affichageMenuGo();
    if(Serial.available()>0){
      if(Serial.read()==0x56){
        go = true;
        menuGO = false;
        lcd.clear();
      }
    }

    if(gone){
      lcd.setCursor(12,1);
      lcd.clear();
      lcd.print("! GO !");
    }
  }

  if(rotation){
    rotation = false;
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
    Serial.write(0x66);
    #ifdef debug
      Serial.println("MENU GO");
    #endif
  }
}

void ISR_clic(){
  clic=true;
}

void ISR_encoder(){
  rotation = true;
}