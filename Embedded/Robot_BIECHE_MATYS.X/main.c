#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "robot.h"
#include "main.h"

    /****************************************************************************************************/
    // Machine à Etat
    /****************************************************************************************************/
 unsigned char stateRobot;

 double vitesse=30; //40 vitesse de fou
 double vitesse2=12;
 
 unsigned char stateRobot;

// double vitesse=30; //40 vitesse de fou
 
void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitesse2, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
            
        case STATE_TOURNE_GAUCHE_PLUS:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitesse2, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_PLUS_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_PLUS_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
            
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(-vitesse2, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
            
         case STATE_TOURNE_DROITE_PLUS:
            PWMSetSpeedConsigne(-vitesse2, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_PLUS_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_PLUS_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
            
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitesse, MOTEUR_GAUCHE); //vitesse de base 15
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-vitesse, MOTEUR_DROIT); //vitesse de base 15
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_RECULE:
            PWMSetSpeedConsigne(-vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitesse, MOTEUR_GAUCHE); //vitesse de base 15
            stateRobot = STATE_RECULE_EN_COURS;
        case STATE_RECULE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}


unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //?Dtermination de la position des obstacles en fonction des ???tlmtres
    if (robotState.distanceTelemetreDroit < SEUIL_OBSTACLE &&
            robotState.distanceTelemetreCentre > SEUIL_OBSTACLE &&
            robotState.distanceTelemetreGauche > SEUIL_OBSTACLE) //Obstacle ?droite
        positionObstacle = OBSTACLE_A_DROITE;
    
    
    else if (robotState.distanceTelemetreDroit > SEUIL_OBSTACLE &&
            robotState.distanceTelemetreCentre > SEUIL_OBSTACLE &&
            robotState.distanceTelemetreGauche < SEUIL_OBSTACLE 
            ) //Obstacle ?gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    
    
    else if (robotState.distanceTelemetreCentre < SEUIL_OBSTACLE) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    
    else if (robotState.distanceTelemetreDroit > SEUIL_OBSTACLE &&
            robotState.distanceTelemetreCentre > SEUIL_OBSTACLE &&
            robotState.distanceTelemetreGauche > SEUIL_OBSTACLE
            ) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
    
    else if (robotState.distanceTelemetreDroit < SEUIL_OBSTACLE &&
            robotState.distanceTelemetrePlusDroit < SEUIL_OBSTACLE)
        positionObstacle = OBSTACLE_A_DROITE_PLUS;
    
    else if (robotState.distanceTelemetreGauche < SEUIL_OBSTACLE &&
            robotState.distanceTelemetrePlusGauche < SEUIL_OBSTACLE)
        positionObstacle = OBSTACLE_A_GAUCHE_PLUS;
    else
        positionObstacle=PARTOUT;
      
    
   
    //?Dtermination de l??tat ?venir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_DROITE_PLUS )
        nextStateRobot = STATE_TOURNE_DROITE_PLUS;
    else if (positionObstacle == OBSTACLE_A_GAUCHE_PLUS )
        nextStateRobot = STATE_TOURNE_GAUCHE_PLUS;
    else
        nextStateRobot = STATE_RECULE;
    //Si l?on n?est pas dans la transition de l??tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}

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
    
    
    
    

    LED_BLANCHE_1 = 0;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 0;

    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;
    
    //unsigned int ADCValue0, ADCValue1, ADCValue2;
   
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

            // Recuperation des valeurs
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetrePlusDroit = 34 / volts - 5;
            volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetrePlusGauche = 34 / volts - 5;

            
            if(robotState.distanceTelemetreGauche < SEUIL_OBSTACLE)
                LED_BLEUE_1 = 1;
            else
                LED_BLEUE_1 = 0;
            if(robotState.distanceTelemetreCentre < SEUIL_OBSTACLE)
                LED_ORANGE_1 = 1;
            else
                LED_ORANGE_1 = 0;
            if(robotState.distanceTelemetreDroit < SEUIL_OBSTACLE)
                LED_ROUGE_1 = 1;
            else
                LED_ROUGE_1 = 0;
            if(robotState.distanceTelemetrePlusDroit < SEUIL_OBSTACLE)
                LED_VERTE_1 = 1;
            else
                LED_VERTE_1 = 0;
            if(robotState.distanceTelemetrePlusGauche < SEUIL_OBSTACLE)
                LED_BLANCHE_1 = 1;
            else
                LED_BLANCHE_1 = 0;            
         
        } 
    }
}