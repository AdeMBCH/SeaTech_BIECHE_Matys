#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();
    /***************************************************************************************************/
    //Initialisation de l'ADC
    /****************************************************************************************************/
    InitADC1();
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
    /****************************************************************************************************/
    // Configuration des entrées sorties
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
    
    unsigned int ADCValue0, ADCValue1, ADCValue2;
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        /*LED_BLANCHE_1 = !LED_BLANCHE_1;
        LED_BLEUE_1 = !LED_BLEUE_1;*/
        
        
        // ADC conversion est-il fini ?
        if (ADCIsConversionFinished()) {
            // Nettoyage du flag
            ADCClearConversionFinishedFlag();

            // Retrieve the conversion results
            unsigned int *result = ADCGetResult();
            ADCValue0 = result[0];
            ADCValue1 = result[1];
            ADCValue2 = result[2];
        }        
        
    } // fin main
}