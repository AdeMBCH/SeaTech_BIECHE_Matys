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
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include <libpic30.h>
#include "UART_Protocol.h"

    /****************************************************************************************************/
    // Machine à Etat
    /****************************************************************************************************/
 unsigned char stateRobot;

 double vitesse=30; //40 vitesse de fou
 double vitesse2=0;
 
 unsigned char stateRobot;
 
void OperatingSystemLoop(void) {
    if (timestamp > 60000){
        PWMSetSpeedConsigne(0, MOTEUR_DROIT);
        PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
    } else {
        switch (stateRobot) {
                case STATE_ATTENTE:
                    //timestamp = 0;
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
}


unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    unsigned char telemetres = 0;

    // Lecture des valeurs des telemetres
    telemetres = (robotState.distanceTelemetrePlusGauche < SEUIL_OBSTACLE)? telemetres | TELEMETRE_GAUCHE_PLUS : telemetres;
    telemetres = (robotState.distanceTelemetreGauche < SEUIL_OBSTACLE)? telemetres | TELEMETRE_GAUCHE : telemetres;
    telemetres = (robotState.distanceTelemetreCentre < SEUIL_OBSTACLE)? telemetres | TELEMETRE_CENTRE : telemetres;
    telemetres = (robotState.distanceTelemetreDroit < SEUIL_OBSTACLE)? telemetres | TELEMETRE_DROITE : telemetres;
    telemetres = (robotState.distanceTelemetrePlusDroit < SEUIL_OBSTACLE)? telemetres | TELEMETRE_DROITE_PLUS : telemetres;

    // Détermination de la position des obstacles en fonction des interferomètres
    switch (telemetres) {
        case 0x00: // Aucun obstacle détecté
            positionObstacle = PAS_D_OBSTACLE;
            break;
        case 0x01: // Obstacle à gauche plus
            positionObstacle = OBSTACLE_A_GAUCHE_PLUS;
            break;
        case 0x02: // Obstacle à gauche
            positionObstacle = OBSTACLE_A_GAUCHE;
            break;
        case 0x03: // Obstacles à gauche plus et gauche
            positionObstacle = OBSTACLE_A_GAUCHE;
            break;
        case 0x04: // Obstacle en face
            positionObstacle = OBSTACLE_EN_FACE;
            break;
        case 0x05: // Obstacles en face et à gauche plus
            positionObstacle = OBSTACLE_EN_FACE;
            break;
        case 0x06: // Obstacles en face et à gauche
            positionObstacle = OBSTACLE_A_GAUCHE;
            break;
        case 0x07: // Obstacles en face, à gauche plus et à gauche
            positionObstacle = OBSTACLE_A_GAUCHE;
            break;
        case 0x08: // Obstacle à droite
            positionObstacle = OBSTACLE_A_DROITE;
            break;
        case 0x09: // Obstacles à droite et à gauche plus
            positionObstacle = OBSTACLE_A_DROITE;
            break;
        case 0x0A: // Obstacles à droite et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x0B: // Obstacles à droite, à gauche plus et à gauche
            positionObstacle = OBSTACLE_A_DROITE;
            break;
        case 0x0C: // Obstacles à droite et en face
            positionObstacle = OBSTACLE_A_DROITE;
            break;
        case 0x0D: // Obstacles à droite, en face et à gauche plus
            positionObstacle = OBSTACLE_A_DROITE;
            break;
        case 0x0E: // Obstacles à droite, en face et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x0F: // Obstacles à droite, en face, à gauche plus et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x10: // Obstacle à droite plus
            positionObstacle = OBSTACLE_A_DROITE_PLUS;
            break;
        case 0x11: // Obstacles à droite plus et à gauche plus
            positionObstacle = PAS_D_OBSTACLE;
            break;
        case 0x12: // Obstacles à droite plus et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x13: // Obstacles à droite plus, à gauche plus et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x14: // Obstacles à droite plus et en face
            positionObstacle = PARTOUT;
            break;
        case 0x15: // Obstacles à droite plus, en face et à gauche plus
            positionObstacle = PARTOUT;
            break;
        case 0x16: // Obstacles à droite plus, en face et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x17: // Obstacles à droite plus, en face, à gauche plus et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x18: // Obstacles à droite et à droite plus
            positionObstacle = OBSTACLE_A_DROITE_PLUS;
            break;
        case 0x19: // Obstacles à droite, à droite plus et à gauche plus
            positionObstacle = PARTOUT;
            break;
        case 0x1A: // Obstacles à droite, à droite plus et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x1B: // Obstacles à droite, à droite plus, à gauche plus et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x1C: // Obstacles à droite, à droite plus et en face
            positionObstacle = PARTOUT;
            break;
        case 0x1D: // Obstacles à droite, à droite plus, en face et à gauche plus
            positionObstacle = PARTOUT;
            break;
        case 0x1E: // Obstacles à droite, à droite plus, en face et à gauche
            positionObstacle = PARTOUT;
            break;
        case 0x1F: // Obstacles partout
            positionObstacle = PARTOUT;
            break;
        default:
            positionObstacle = PAS_D_OBSTACLE;
            break;
    }

    // Détermination de l'état suivant du robot
    switch (positionObstacle) {
        case PAS_D_OBSTACLE:
            nextStateRobot = STATE_AVANCE;
            break;
        case OBSTACLE_A_GAUCHE_PLUS:
            nextStateRobot = STATE_TOURNE_DROITE_PLUS;
            break;
        case OBSTACLE_A_GAUCHE:
            nextStateRobot = STATE_TOURNE_DROITE;
            break;
        case OBSTACLE_EN_FACE:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case OBSTACLE_A_DROITE:
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;
        case OBSTACLE_A_DROITE_PLUS:
            nextStateRobot = STATE_TOURNE_GAUCHE_PLUS;
            break;
        case PARTOUT:
            nextStateRobot = STATE_RECULE;
            break;
        default:
            nextStateRobot = STATE_ATTENTE;
            break;
    }

    // Si l'on n'est pas dans la transition de l'étape en cours
    if (nextStateRobot!= stateRobot - 1)
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
    
    InitUART();
    
    
    
    

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
        
        //int i;
        //for(i=0; i< CB_RX1_GetDataSize(); i++){
        //    unsigned char c = CB_RX1_Get();
            
        //    SendMessage(&c,1);
        //}
        
        
        
        //unsigned char payload[] = {'B', 'o', 'n', 'j', 'o', 'u', 'r'};

        //__delay32(100000);
        
        SendIrDistanceMessage(robotState.distanceTelemetreGauche,robotState.distanceTelemetreCentre,robotState.distanceTelemetreDroit);
        SendMotorSpeedMessage(robotState.vitesseGaucheConsigne,robotState.vitesseDroiteConsigne);
        __delay32(40000000);
        
        
        
        //SendMessage((unsigned char*) "Test",4);
        
        //SendMessageDirect((unsigned char*) "Bonjour", 7);
        //__delay32(40000000);
    }
}