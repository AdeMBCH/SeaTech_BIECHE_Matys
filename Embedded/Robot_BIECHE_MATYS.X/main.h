#ifndef MAIN_H
#define	MAIN_H

#define FREQUENCE_TIMER1 10000
#define FREQUENCE_TIMER4 1000
#define FCY 60000000

#define SEUIL_OBSTACLE 35

#define STATE_ATTENTE 0
#define STATE_ATTENTE_EN_COURS 1
#define STATE_AVANCE 2
#define STATE_AVANCE_EN_COURS 3
#define STATE_TOURNE_GAUCHE 4
#define STATE_TOURNE_GAUCHE_EN_COURS 5
#define STATE_TOURNE_DROITE_PLUS 6
#define STATE_TOURNE_DROITE_PLUS_EN_COURS 7
#define STATE_TOURNE_GAUCHE_PLUS  8
#define STATE_TOURNE_GAUCHE_PLUS_EN_COURS 9
#define STATE_TOURNE_DROITE 10
#define STATE_TOURNE_DROITE_EN_COURS 11
#define STATE_TOURNE_SUR_PLACE_GAUCHE 12
#define STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS 13
#define STATE_TOURNE_SUR_PLACE_DROITE 14
#define STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS 15
#define STATE_ARRET 16
#define STATE_ARRET_EN_COURS 17
#define STATE_RECULE 18
#define STATE_RECULE_EN_COURS 19

#define OBSTACLE_A_GAUCHE_PLUS   1
#define OBSTACLE_A_GAUCHE        2
#define OBSTACLE_EN_FACE         3
#define OBSTACLE_A_DROITE        4
#define OBSTACLE_A_DROITE_PLUS   5
#define PAS_D_OBSTACLE           6
#define PARTOUT                  7

#define TELEMETRE_GAUCHE_PLUS  0x01
#define TELEMETRE_GAUCHE       0x02
#define TELEMETRE_CENTRE       0x04
#define TELEMETRE_DROITE       0x08
#define TELEMETRE_DROITE_PLUS  0x10

void OperatingSystemLoop(void);
void SetNextRobotStateInAutomaticMode();


#endif	/* MAIN_H */

