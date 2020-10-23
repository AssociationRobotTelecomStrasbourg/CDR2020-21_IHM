#ifndef MENUS_H
#define MENUS_H

#include <Arduino.h>
#include "LiquidCrystal_I2C.h"

void affichageMenu0();
void affichageMenuGo();

void menuMEF(int &posEncoder, bool &clic, uint8_t &menuA, uint8_t &menuB, bool refreshScreen);

void displayCursor(const int &posEncoder, const uint8_t &menuA, LiquidCrystal_I2C &lcd);
void displayMenu0(LiquidCrystal_I2C &lcd);
void displayMenuTest0(LiquidCrystal_I2C &lcd);

void displayMenuTest1();
void displayMenuTest2();
void displayMenuTest3();
void displayMenuTest4();
#endif