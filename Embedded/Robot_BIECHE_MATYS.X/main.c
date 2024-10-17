#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();
    /***************************************************************************************************/
    //Initialisation des Timers
    /****************************************************************************************************/
    InitTimer1();
    InitTimer23();
    /***************************************************************************************************/
    //Initialisation des moteurs
    /****************************************************************************************************/
    InitPWM();
    //PWMSetSpeed(0,MOTEUR_DROIT);
    PWMSetSpeedConsigne(0,MOTEUR_GAUCHE);
    PWMSetSpeedConsigne(0,MOTEUR_DROIT);
    /****************************************************************************************************/
    // Configuration des entr�es sorties
    /****************************************************************************************************/
    InitIO();

    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;

    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        LED_BLANCHE_1 = !LED_BLANCHE_1;
        LED_BLEUE_1 = !LED_BLEUE_1;
    } // fin main
}