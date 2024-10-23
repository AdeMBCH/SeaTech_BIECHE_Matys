
#ifndef PWM_H
#define	PWM_H

#define ACCELERATION 10
#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1

void InitPWM(void);
//void PWMSetSpeed(float,unsigned char);
void PWMSetSpeedConsigne(float,unsigned char);
void PWMUpdateSpeed();
#endif	

