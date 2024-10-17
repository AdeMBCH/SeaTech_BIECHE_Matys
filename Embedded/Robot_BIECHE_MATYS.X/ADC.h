#ifndef ADC_H
#define	ADC_H






void InitADC1(void);
void ADC1StartConversionSequence();
unsigned int * ADCGetResult(void);
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);
#endif	/* ADC_H */

