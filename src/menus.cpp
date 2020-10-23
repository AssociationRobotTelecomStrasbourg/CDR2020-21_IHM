#include <Arduino.h>
#include "menus.h"

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
void menuMEF(int &posEncoder, bool &clic, bool &menuA, bool &menuB){

}