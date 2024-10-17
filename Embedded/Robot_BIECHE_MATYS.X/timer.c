#include <xc.h>
#include "timer.h"
#include "IO.h"
#include "PWM.h"
#include "ADC.h"

//Initialisation d?un timer 32 bits
void InitTimer23(void) {
T3CONbits.TON = 0; // Stop any 16-bit Timer3 operation
T2CONbits.TON = 0; // Stop any 16/32-bit Timer3 operation
T2CONbits.T32 = 1; // Enable 32-bit Timer mode
T2CONbits.TCS = 0; // Select internal instruction cycle clock
T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
TMR3 = 0x00; // Clear 32-bit Timer (msw)
TMR2 = 0x00; // Clear 32-bit Timer (lsw)
//PR3 = 0x0262; // Load 32-bit period value (msw)
//PR2 = 0x5A00; // Load 32-bit period value (lsw)
PR2 =0x0E00;
PR3 = 0x727;
IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
IEC0bits.T3IE = 1; // Enable Timer3 interrupt
T2CONbits.TON = 1; // Start 32-bit Timer
/* Example code for Timer3 ISR */
}

//Interruption du timer 32 bits sur 2-3
//void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
//IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
//LED_ORANGE_1 = !LED_ORANGE_1;
//}

//Initialisation d?un timer 16 bits
void InitTimer1(void)
{
//Timer1 pour horodater les mesures (1ms)
T1CONbits.TON = 0; // Disable Timer
T1CONbits.TCKPS = 0b01; //Prescaler
//11 = 1:256 prescale value
//10 = 1:64 prescale value
//01 = 1:8 prescale value
//00 = 1:1 prescale value
T1CONbits.TCS = 0; //clock source = internal clock
//PR1 = 0x1D4C;
//PR1 = 0b1110101001100;
//PR1=7500;

    // Calcul de la période pour une interruption 100 Hz
    // Assuming FCY = 60 MHz and prescaler = 1:8
    // PR1 = (FCY / (prescaler * desired_frequency)) - 1
    // PR1 = (60,000,000 / (8 * 100)) - 1 = 74999

//PR1=0x249F0; //réponse à la question

    //PR1 pour moteurs
PR1=74999;


IFS0bits.T1IF = 0; // Clear Timer Interrupt Flag
IEC0bits.T1IE = 1; // Enable Timer interrupt
T1CONbits.TON = 1; // Enable Timer
}

//Interruption du timer 1
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    //PWMUpdateSpeed();
    ADC1StartConversionSequence();
}
unsigned char toggle=0;
//Interruption pour moteurs
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    if(toggle == 0){
        PWMSetSpeedConsigne(20,MOTEUR_GAUCHE);
        PWMSetSpeedConsigne(20,MOTEUR_DROIT);
        toggle = 1;
    }else{
        PWMSetSpeedConsigne(-20,MOTEUR_GAUCHE);
        PWMSetSpeedConsigne(-20,MOTEUR_DROIT);
        toggle = 0;
    }
}
