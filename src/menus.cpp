#include <Arduino.h>
#include "menus.h"
#include "LiquidCrystal_I2C.h"
#include "debug.h"
/* 
Plan de navigation /menus
--------------------------------------
(n,m) : valeur de variable menuA:menuB
Ecran initialisation (0:0) -> Menu principal(1:0):
                        -> Go Match(2:0) -> Config côté de l'arène(2:1) -> Attente de départ(2:2)
                        -> Menu Tests (3:0):
                                -> Test 1 (3:1)
                                    -
                                    -
                                    -
                                    -Retour vers menu Test
                                -> Test 2 (3:2)
                                    -
                                    -
                                    -
                                    -Retour vers menu Test
                                -> Test 3 (3:3)
                                    -
                                    -
                                    -
                                    -Retour vers menu Test
                                -> Test 4 (3:4)
                                    -
                                    -
                                    -
                                    -Retour vers menu Test
                                -> Retour vers menu principal (posEncoder = 5)
--------------------------------------
 */
void menuMEF(int &posEncoder, bool &clic, uint8_t &menuA, uint8_t &menuB, bool &refreshScreen, LiquidCrystal_I2C &lcd, bool &areneSide, bool &goMatch){
    //(A:B) = (0:0) -> Menu d'initialisation
    #ifdef debug
        Serial.print("MenuA|MenuB|refreshScreen: ");
        Serial.print(menuA);
        Serial.print("|");
        Serial.print(menuB);
        Serial.print("|");
        Serial.println(refreshScreen);
        Serial.print("posEncoder : ");
        Serial.println(posEncoder);
        delay(100);
    #endif
    if(menuA == 0 && menuB == 0 && clic){
        //On sort de l'ecran d'initialisation
        menuA = 1;
        clic = 0;
        refreshScreen = true;
    }
    if(menuA == 1 && menuB == 0){
        /* Pour le menu principal */
        if(refreshScreen){
            /* Pour éviter l'effet de clignotement, on doit faire en sorte 
            que les caractères fixes de l'écran ne soit rafraichis qu'une seule fois, a chaque transition */
            refreshScreen = false;
            lcd.clear();
            displayMenu0(lcd);
        }
        if(clic){
            //Lorsqu'il y a un clic, on regarde où est placé le curseur
            clic = 0;
            switch (posEncoder)
            {
            case 0:
                /* On est sur la ligne "GoMatch" */
                menuA = 2;
                refreshScreen = true;
                break;

            case 1:
                /* On est sur la ligne "Menu Test" */
                menuA = 3;
                refreshScreen = true;
                #ifdef debug
                    Serial.println("Vers menu test....");
                    Serial.print("refresh: ");
                    Serial.println(refreshScreen);
                #endif
                break;
            
            default:
                break;
            }
        }
    }
    if(menuA == 2 && menuB == 0){
        //Menu GoMatch
        if(refreshScreen){
            refreshScreen = false;
            lcd.clear();
            displayGoMatch(lcd, areneSide);
        }
        if(clic){
            clic = 0;
            switch (posEncoder)
            {
            case 0:
                /* Alors on change de côté de l'arène */
                refreshScreen = 1;
                areneSide = !areneSide;
                break;
            case 2:
                /* Le match commence */
                goMatch = true;
                refreshScreen = 1;
            case 3:
                /* Retour menu principal */
                refreshScreen = 1;
                menuB = 0;
                menuA = 1;
            default:
                break;
            }
        }
    }
    if(menuA == 3 && menuB == 0){
        //Menu Test principal
        if(refreshScreen){
            refreshScreen = false;
            lcd.clear();
            displayMenuTest0(lcd);
        }
        if(clic){
            clic = 0;
            switch (posEncoder)
            {
            case 0:
                /* Test lidars fixes */
                menuB = 1;
                refreshScreen = true;
                break;
            case 1:
                /* Test servos */
                menuB = 2;
                refreshScreen = true;
                break;
            case 2:
                /* test steppers */
                menuB = 3;
                refreshScreen = true;
                break;
            case 3:
                /* bouton retour */
                menuA = 1;
                menuB = 0;
                refreshScreen = true;
                break;
            
            default:
                break;
            }
        }
    }
}

void displayCursor(int &posEncoder, const uint8_t &menuA, LiquidCrystal_I2C &lcd){
    //Fonction permettant d'afficher dynamiquement le curseur hors de l'écran d'initialisation
    //Gère egalement l'intervalle de validité de posEncoder
    if(menuA!=0){
        lcd.cursor_on();
        lcd.setCursor(0, posEncoder);
    }
    if(posEncoder < 0){
        posEncoder = 0;
    }
    else if(posEncoder > 3){
        posEncoder = 3;
    }
    
}

void displayMenu0(LiquidCrystal_I2C &lcd){
    lcd.setCursor(1,0);
    lcd.print("GO Match");
    lcd.setCursor(1,1);
    lcd.print("Menu Tests");
}

void displayMenuTest0(LiquidCrystal_I2C &lcd){
    lcd.setCursor(1,0);
    lcd.print("Lidars Fixes");
    lcd.setCursor(1,1);
    lcd.print("Test Servos");
    lcd.setCursor(1,2);
    lcd.print("Test Steppers");
    lcd.setCursor(1,3);
    lcd.print("RETOUR");
}

void displayGoMatch(LiquidCrystal_I2C &lcd, bool areneSide){
    lcd.setCursor(1,0);
    lcd.print("Side : ");
    switch (areneSide)
    {
    case false:
        /* On est du côté gauche */
        lcd.print("Gauche");
        break;
    case true:
        /* On est du côté droit */
        lcd.print("Droit ");
        break;
    default:
        break;
    }

    lcd.setCursor(1,2);
    lcd.print("GO MATCH ! ");
    lcd.setCursor(1,3);
    lcd.print("RETOUR");
}