#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "robot.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();
    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();
    /***************************************************************************************************/
    //Initialisation de l'ADC
    /****************************************************************************************************/
    InitADC1();
    /***************************************************************************************************/
    //Initialisation des Timers
    /****************************************************************************************************/
    InitTimer1();
    InitTimer4();
//    InitTimer23();
    /***************************************************************************************************/
    //Initialisation des moteurs
    /****************************************************************************************************/
    InitPWM();
    //PWMSetSpeed(0,MOTEUR_DROIT);

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
        if (ADCIsConversionFinished()==1) {
            // Nettoyage du flag
            ADCClearConversionFinishedFlag();

            // Retrieve the conversion results
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;

        } 
    }
}